__kernel void gradx( __write_only image2d_t out, __read_only image2d_t src, __local float4* buf  )
{
	const sampler_t sampler = CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_CLAMP | CLK_FILTER_NEAREST;
	const int gx = get_global_id( 0 );
	const int gy = get_global_id( 1 );
	const int lx = get_local_id( 0 );
	const int ly = get_local_id( 1 );
	const int lw = get_local_size( 0 );
	const int lh = get_local_size( 1 );
	const int width = get_image_width( out );
	const int height = get_image_height( out );
	const int2 base = { get_group_id( 0 ) * lw - 1, get_group_id( 1 ) * lh };
	const int bstride = lw + 2;
	float2 coord;
	float4 dx;

	for( int x = lx; x < lw + 2; x += lw ) {
		buf[ mul24( ly, bstride ) + x ] = read_imagef( src, sampler, base + ( int2 )( x, ly ) );
	}

	barrier( CLK_LOCAL_MEM_FENCE );

#define BUF( x, y ) buf[ mul24( y , bstride ) + ( x ) + 1 ]

	if( gx >= width || gy >= height )
		return;

	dx = BUF( lx + 1, ly ) - BUF( lx - 1, ly  );
	write_imagef( out,( int2 )( gx, gy ), dx );
}
