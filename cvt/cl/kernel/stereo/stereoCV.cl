inline float costRGB_L1( const float4 i1, const float4 i2 )
{
	const float4 dotmul = { 0.333333, 0.333333, 0.333333, 0 };
	return dot( fabs( i2 - i1 ), dotmul );
}

inline float costRGB_L2( const float4 i1, const float4 i2 )
{
	const float4 dotmul = { 0.333333, 0.333333, 0.333333, 0 };
	float4 tmp = i2 - i1;
	tmp.w = 0;
	return fast_length( tmp );
}

inline float costGRAY_L1( const float4 i1, const float4 i2 )
{
	return fabs( i2.x - i1.x );
}

inline float costGRAY_L2( const float4 i1, const float4 i2 )
{
	float tmp = i2.x - i2.y;
	return tmp * tmp;
}

__kernel void stereoCV( global float* cv, int depth, __read_only image2d_t src1, __read_only image2d_t src2, __local float4* buf  )
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
	const int stride = mul24( width, height );
	float4 pixel;
	global float* cvptr = cv + ( mul24( gy, width ) + gx );

	/* Fill the local buffer with data from src2 */
	for( int d = lx; d < buflen; d += lw )
		buf[ d ] = read_imagef( src2, sampler, ( int2 )( base + d, gy ) );

	barrier( CLK_LOCAL_MEM_FENCE );

	/* we assume src1.width == src2.width == cv.width */
	if( gx >= width )
		return;

	/* read current pixel from src1 */
	pixel = read_imagef( src1, sampler, ( int2 ) ( gx, gy ) );

	/* store the result of the cost function */
	for( int d = 0; d < depth; d++ )
		cvptr[ d * stride ] = costRGB_L1( pixel, buf[ lx + d ] );
}

__kernel void stereoCV_WTA( __write_only image2d_t dmap, global const float* cv, int depth )
{
	const int gx = get_global_id( 0 );
	const int gy = get_global_id( 1 );
	const int width = get_image_width( dmap );
	const int height = get_image_height( dmap );
	const int stride = mul24( width, height );
	global float* cvptr = cv + ( mul24( gy, width ) + gx );
	int idx, cmp;
	float val, nval;

	if( gx >= width || gy >= height )
		return;

	idx = 0;
	val = cvptr[ 0 ];

	for( int d = 1; d < depth; d++ ) {
		nval = cvptr[ d * stride ];
		cmp = isless( nval, val );
		val = select( val, nval, cmp );
		idx = select( idx, d, cmp );
	}

	val = ( float ) idx / ( float ) depth;
	write_imagef( dmap, ( int2 ) ( gx, gy ), ( float4 ) ( val, val, val, 1.0f ) );
}
