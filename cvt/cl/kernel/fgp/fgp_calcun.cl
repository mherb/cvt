__kernel void fgp_calcun( __write_only image2d_t out, __write_only image2d_t n, __read_only image2d_t img, __read_only image2d_t e1, __read_only image2d_t e0, const float lambda, const float t, __local float4* buf  )
{
    const sampler_t sampler = CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_CLAMP_TO_EDGE | CLK_FILTER_NEAREST;
    int2 coord, coord2;
	const int lx = get_local_id( 0 );
    const int ly = get_local_id( 1 );
    const int lw = get_local_size( 0 );
    const int lh = get_local_size( 1 );
	const int width = get_image_width( out );
	const int height = get_image_height( out );
	float4 nx, ny, ndx, ndy, val;

    coord.x = get_global_id( 0 );
    coord.y = get_global_id( 1 );

	if( coord.x >= width || coord.y >= height )
		return;


	coord2.x = mul24( coord.x, 2 );
	coord2.y = coord.y;

	nx = read_imagef( e1, sampler, coord2 );
	val = read_imagef( e0, sampler, coord2 );
	nx = nx + t * ( nx - val );
	ny = read_imagef( e1, sampler, coord2 + ( int2 ) ( 1, 0 ) );
	val = read_imagef( e0, sampler, coord2 + ( int2 ) ( 1, 0 ) );
	ny = ny + t * ( ny - val );

	write_imagef( n, coord2, nx );
	write_imagef( n, coord2 + ( int2 ) ( 1, 0 ), ny );

#define XBUF( x, y ) buf[ mul24( mul24( ( y + 1 ), ( lw + 1 ) ) + ( x + 1 ), 2 ) + 0 ]
#define YBUF( x, y ) buf[ mul24( mul24( ( y + 1 ), ( lw + 1 ) ) + ( x + 1 ), 2 ) + 1 ]

	XBUF( lx, ly ) = nx;
	YBUF( lx, ly ) = ny;

	if( lx == 0 ) {
		float4 tmp;
		tmp = read_imagef( e1, sampler, coord2 - ( int2 )( 2, 0 ) );
		val = read_imagef( e0, sampler, coord2 - ( int2 )( 2, 0 ) );
		XBUF( lx - 1, ly ) = tmp + t * ( tmp - val );
	}
	if( ly == 0 ) {
		float4 tmp;
		tmp = read_imagef( e1, sampler, coord2 + ( int2 )( 1, -1 ) );
		val = read_imagef( e0, sampler, coord2 + ( int2 )( 1, -1 ) );
		YBUF( lx, ly - 1 ) = tmp + t * ( tmp - val );
	}

	barrier( CLK_LOCAL_MEM_FENCE );

	if( coord.x == 0 )
		ndx = nx;
	else if( coord.x == width - 1 )
		ndx = - XBUF( lx - 1, ly );
	else
		ndx = nx - XBUF( lx - 1, ly );

	if( coord.y == 0 )
		ndy = ny;
	else if( coord.y == height - 1 )
		ndy = - YBUF( lx, ly - 1 );
	else
		ndy = ny - YBUF( lx, ly - 1 );

    val = read_imagef( img, sampler, coord );
	val = val - lambda * ( ndx + ndy );
    write_imagef( out, coord, val );
}
