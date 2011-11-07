__kernel void CALCU( __write_only image2d_t out, __read_only image2d_t img, __read_only image2d_t n , const float lambda, __local float4* buf  )
{
    const sampler_t sampler = CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_CLAMP_TO_EDGE | CLK_FILTER_NEAREST;
    int2 coord, coord2;
	const int lx = get_local_id( 0 );
    const int ly = get_local_id( 1 );
    const int lw = get_local_size( 0 );
    const int lh = get_local_size( 1 );
	const int width = get_global_size( 0 );
	const int height = get_global_size( 1 );
	float4 nx, ny, ndx, ndy, val;

    coord.x = get_global_id( 0 );
    coord.y = get_global_id( 1 );


	coord2.x = mul24( coord.x, 2 );
	coord2.y = coord.y;

	nx = read_imagef( n, sampler, coord2 );
	ny = read_imagef( n, sampler, coord2 + ( int2 ) ( 1, 0 ) );

#define XBUF( x, y ) buf[ mul24( mul24( ( y + 1 ), ( lw + 1 ) ) + ( x + 1 ), 2 ) + 0 ]
#define YBUF( x, y ) buf[ mul24( mul24( ( y + 1 ), ( lw + 1 ) ) + ( x + 1 ), 2 ) + 1 ]

	XBUF( lx, ly ) = nx;
	YBUF( lx, ly ) = ny;

	if( lx == 0 )
		XBUF( lx - 1, ly ) = read_imagef( n, sampler, coord2 - ( int2 )( 2, 0 ) );
	if( ly == 0 )
		YBUF( lx, ly - 1 ) = read_imagef( n, sampler, coord2 + ( int2 )( 1, -1 ) );

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
