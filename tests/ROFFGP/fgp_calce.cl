__kernel void fgp_calce( __write_only image2d_t out, __read_only image2d_t u, __read_only image2d_t n , const float ilambda, __local float4* buf  )
{
    const sampler_t sampler = CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_CLAMP_TO_EDGE | CLK_FILTER_NEAREST;
    int2 coord, coord2;
	const int lx = get_local_id( 0 );
    const int ly = get_local_id( 1 );
    const int lw = get_local_size( 0 );
    const int lh = get_local_size( 1 );
	const int width = get_image_width( out );
	const int height = get_image_height( out );
	float4 nx, ny, dx, dy, val, norm;

    coord.x = get_global_id( 0 );
    coord.y = get_global_id( 1 );

	if( coord.x >= width || coord.y >= height )
		return;

    val = read_imagef( u, sampler, coord );

	coord2.x = mul24( coord.x, 2 );
	coord2.y = coord.y;

#define BUF( x, y ) buf[ mul24( y, ( lw + 1 ) ) + ( x ) ]

	BUF( lx, ly ) = val;

	if( lx == lw - 1 )
		BUF( lx + 1, ly ) = read_imagef( u, sampler, coord + ( int2 ) ( 1, 0 ) );
	if( ly == lh - 1 )
		BUF( lx, ly + 1 ) = read_imagef( u, sampler, coord + ( int2 ) ( 0, 1 ));

	barrier( CLK_LOCAL_MEM_FENCE );

	nx = read_imagef( n, sampler, coord2 );
	ny = read_imagef( n, sampler, coord2 + ( int2 ) ( 1, 0 ) );

	dx = val - BUF( lx + 1, ly );
	dy = val - BUF( lx, ly + 1 );

	nx = nx + ilambda * dx;
	ny = ny + ilambda * dy;

	norm = fmin( ( float4 ) 1.0f, native_rsqrt( nx * nx + ny * ny ) );
	nx = nx * norm;
	ny = ny * norm;

	write_imagef( out, coord2, nx );
	write_imagef( out, coord2 + ( int2 ) ( 1, 0 ), ny );
}

