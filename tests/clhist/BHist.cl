#define MASK 0xFFFFFFFFUL

float bspline( float x )
{
	x = fabs( x );
	if( x > 2.0f )
		return 0.0f;
	if( x > 1.0f ) {
		x = ( 2.0f - x );
		return x * x * x / 6.0f;
	}
	return ( 4.0f - 6.0f * x * x + 3.0f * x * x *x ) / 6.0f;
}

void incrementHistogram( __local float* dst, float4 values )
{
	*( dst - 1 ) += values.x;
	*( dst + 0 ) += values.y;
	*( dst + 1 ) += values.z;
	*( dst + 2 ) += values.w;
}

#define XLEN 4
#define YLEN 4
#define XYSIZE ( XLEN * YLEN )

__kernel __attribute__ ( (reqd_work_group_size( XLEN, YLEN, 1 ) ) )
void bhist( __global uint* hist, __global uint* jhist, const int len, __read_only image2d_t template, __read_only image2d_t warped, __local float* lhist, __local float* ljhist )
{
	const sampler_t sampler = CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_CLAMP | CLK_FILTER_NEAREST;
	size_t id = get_local_id( 0 ) + get_local_size( 0 ) * get_local_id( 1 );
//	size_t gid = get_global_id( 0 ) + get_global_size( 0 ) * get_global_id( 1 );
	int2 coord;

	for( int i = 0; i < len; i++ )
		lhist[ id * ( len + 1 ) + i ] = 0;

	for( int i = 0; i < ( len + 1 ) * ( len + 1 ); i++ )
		ljhist[ id * ( ( len + 1 ) * ( len + 1 ) ) + i ] = 0;

	for( int ly = 0; ly < YLEN; ly++ ) {
		coord.y = get_global_id( 1 ) * YLEN + ly;
		for( int lx = 0; lx < XLEN; lx++ ) {
			coord.x = get_global_id( 0 ) * XLEN + lx;
			if( coord.x < get_image_width( template ) && coord.y < get_image_height( template ) ) {
				// update histogram of warped image
				float v = read_imagef( warped, sampler, coord ).x;
				float fidx = v * ( float ) ( len - 3 ) + 1.0f;
				float idx = floor( fidx );
				float4 values;
				values.x = bspline( - fidx + idx - 1.0f );
				values.y = bspline( - fidx + idx + 0.0f );
				values.z = bspline( - fidx + idx + 1.0f );
				values.w = bspline( - fidx + idx + 2.0f );

				int i = ( int ) idx;
				incrementHistogram( lhist + id * ( len + 1 ) + i, values  );

				// update joint histogram
				float v2 = read_imagef( template, sampler, coord ).x;
				float fidx2 = v2 * ( float ) ( len - 3 ) + 1.0f;
				float idx2 = floor( fidx2 );
				float4 values2;
				values2.x = bspline( - fidx2 + idx2 - 1.0f );
				values2.y = bspline( - fidx2 + idx2 + 0.0f );
				values2.z = bspline( - fidx2 + idx2 + 1.0f );
				values2.w = bspline( - fidx2 + idx2 + 2.0f );

				int i2 = ( int ) idx2;

				incrementHistogram(	ljhist + id * ( ( len + 1 ) * ( len + 1 ) ) + ( len + 1 ) * ( i2 - 1 ) + i, values * values2.x  );
				incrementHistogram(	ljhist + id * ( ( len + 1 ) * ( len + 1 ) ) + ( len + 1 ) * ( i2 + 0 ) + i, values * values2.y  );
				incrementHistogram(	ljhist + id * ( ( len + 1 ) * ( len + 1 ) ) + ( len + 1 ) * ( i2 + 1 ) + i, values * values2.z  );
				incrementHistogram(	ljhist + id * ( ( len + 1 ) * ( len + 1 ) ) + ( len + 1 ) * ( i2 + 2 ) + i, values * values2.w  );
			}
		}
	}

	barrier( CLK_LOCAL_MEM_FENCE );

	if( id < len ) {
		uint v;
		float sum = 0;

		for( int i = 0; i < XYSIZE; i++ )
			sum += lhist[ i * ( len + 1 ) + id  ];
		v = ( uint ) ( sum * 0x100 );
		atomic_add( hist + id, v );

		for( int k = 0; k < len; k++ ) {
			sum = 0;
			for( int i = 0; i < XYSIZE; i++ )
				sum += ljhist[ i * ( ( len + 1 ) * ( len + 1 ) ) +  id * ( len + 1 ) + k  ];
			v = ( uint ) ( sum * 0x100 );
			atomic_add( jhist + id * ( len + 1 ) + k, v );
		}
	}
}
