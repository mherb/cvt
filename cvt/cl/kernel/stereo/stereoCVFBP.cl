#define FBP_LAMBDA 0.01f
#define FBP_DATATRUNC 0.08f
#define FBP_DISCPENALTY 1.0f
#define FBP_DISCTRUNC 5.0f
#define FBP_INF	1e20f

__kernel void stereoCV_FBP_WTA( __write_only image2d_t dmap, global const float2* cv, const int depth )
{
	const int gx = get_global_id( 0 );
	const int gy = get_global_id( 1 );
	const int width = get_image_width( dmap );
	const int height = get_image_height( dmap );
	const int zstride = mul24( width, height );
	global const float2* cvptr = cv + ( mul24( gy, width ) + gx );
	int idx, cmp;
	float val, nval;

	if( gx >= width || gy >= height )
		return;

	idx = 0;
	val = cvptr->y;

	for( int d = 1; d < depth; d++ ) {
		nval = cvptr->y;
		cmp = isless( nval, val );
		val = select( val, nval, cmp );
		idx = select( idx, d, cmp );
		cvptr += zstride;
	}

	val = ( float ) idx / ( float ) depth;
	write_imagef( dmap, ( int2 ) ( gx, gy ), ( float4 ) ( val, val, val, 1.0f ) );
}

__kernel void stereoCV_FBP_AD( global float2* cv, const int depth, __read_only image2d_t src1, __read_only image2d_t src2, __local float* buf  )
{
	const sampler_t sampler = CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_CLAMP | CLK_FILTER_NEAREST;
	const int lx = get_local_id( 0 );
	const int lw = get_local_size( 0 );
	const int gid = get_group_id( 0 );
	const int gx = get_global_id( 0 );
	const int gy = get_global_id( 1 );
	const int base = mul24( gid, lw );
	const int width = get_image_width( src1 );
	const int height = get_image_height( src2 );
	const int buflen = depth + lw;
	const int zstride = mul24( width, height );
	float pixel;
	global float2* cvptr = cv + mul24( gy, width ) + gx;

	/* Fill the local buffer with data from src2 */
	for( int d = lx; d < buflen; d += lw )
		buf[ d ] = read_imagef( src2, sampler, ( int2 )( base + d, gy ) ).x;

	barrier( CLK_LOCAL_MEM_FENCE );

	/* we assume src1.width == src2.width == cv.width */
	if( gx >= width )
		return;

	/* read current pixel from src1 */
	pixel = read_imagef( src1, sampler, ( int2 ) ( gx, gy ) ).x;

	/* store the result of the cost function */
	for( int d = 0; d < depth; d++ ) {
		*cvptr = ( float2 ) ( FBP_LAMBDA * min( fabs( pixel - buf[ lx + d ] ), FBP_DATATRUNC ), 0.0f );
		cvptr += zstride;
	}
}



inline void stereoCV_FBP_distanceTransformMinNormalize( float* buf, const int depth )
{
	float minval;
	float sum = 0;

	for( int d = 1; d < depth; d++ ) {
		float prev = buf[ d - 1 ] + FBP_DISCPENALTY;
		buf[ d ] = min( buf[ d ], prev );
	}

	minval = buf[ depth - 1 ];
	for( int d = depth-2; d >= 0; d-- )	{
		float prev = buf[ d + 1 ] + FBP_DISCPENALTY;
		buf[ d ] = min( buf[ d ], prev );
		minval = min( minval, buf[ d ] );
	}

	minval += FBP_DISCTRUNC;
	for( int d = 0; d < depth; d++ ) {
		buf[ d ] = min( minval, buf[ d ] );
		sum = sum + buf[ d ];
	}

	if( sum > 0.0f ) {
		for( int d = 0; d < depth; d++ ) {
			buf[ d ] /= sum;
		}
	}
}

__kernel void stereoCV_FBP( global float2* cv, const int width, const int height, const int depth, const int t )
{
	const int gy = get_global_id( 1 );
	const int gx = get_global_id( 0 ) * 2 + ( ( gy + t ) % 2 );
	const int ystride = width;
	const int zstride = mul24( width, height );
	global float2* cvptr = cv + mul24( gy, width ) + gx;
	global const float2* cvptr1 = cvptr + 1;
	global const float2* cvptr2 = cvptr - 1;
	global const float2* cvptr3 = cvptr + ystride;
	global const float2* cvptr4 = cvptr - ystride;
	float buf[ 100 ];

	if( gx < 1 || gy < 1 || gx >= width - 1 || gy >= height - 1 )
		return;

	for( int d = 0; d < depth; d++ ) {
		buf[ d ] = cvptr->x - cvptr->y // current pixel
				   + cvptr1->y // right
				   + cvptr2->y // left
				   + cvptr3->y // top
				   + cvptr4->y; // bottom

		cvptr += zstride;
		cvptr1 += zstride;
		cvptr2 += zstride;
		cvptr3 += zstride;
		cvptr4 += zstride;
	}

	stereoCV_FBP_distanceTransformMinNormalize( buf, depth );

	cvptr = cv + mul24( gy, width ) + gx;
	for( int d = 0; d < depth; d++ ) {
		cvptr->y = buf[ d ]; // update belief
		cvptr += zstride;
	}
}
