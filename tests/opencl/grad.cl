__kernel void GRAD( __write_only image2d_t g2, __write_only image2d_t gx, __write_only image2d_t gy,  __read_only image2d_t in, __local float4* buf )
{
	const sampler_t sampler = CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_CLAMP_TO_EDGE | CLK_FILTER_NEAREST;
	int2 coord;
	float4 dx, dy, d2;
	float4 x1, x2, x3;
	const int lx = get_local_id( 0 );
	const int ly = get_local_id( 1 );
	const int lw = get_local_size( 0 );
	const int lh = get_local_size( 1 );

	coord.x = get_global_id( 0 );
	coord.y = get_global_id( 1 );

#define BUF( x, y ) buf[ mul24( ( ( y ) + 1 ), ( lw + 2 ) ) + ( x ) + 1 ]
	BUF( lx, ly ) = read_imagef( in, sampler, coord );

	if( lx == lw - 1 ) {
		BUF( lx + 1, ly ) = read_imagef( in, sampler, coord + ( int2 )( 1, 0 ) );
		if( ly == lh - 1 )
			BUF( lx + 1, ly + 1 ) = read_imagef( in, sampler, coord + ( int2 )( 1, 1 ) );
		else if( ly == 0 )
			BUF( lx + 1, ly - 1 ) = read_imagef( in, sampler, coord + ( int2 )( 1, -1 ) );
	} else if( lx == 0 ) {
		BUF( lx - 1, ly ) = read_imagef( in, sampler, coord - ( int2 )( 1, 0 ) );
		if( ly == lh - 1 )
			BUF( lx - 1, ly + 1 ) = read_imagef( in, sampler, coord + ( int2 )( -1, 1 ) );
		else if( ly == 0 )
			BUF( lx - 1, ly - 1 ) = read_imagef( in, sampler, coord - ( int2 )( 1, 1 ) );
	}

	if( ly == lh - 1 ) {
		BUF( lx, ly + 1 ) = read_imagef( in, sampler, coord + ( int2 )( 0, 1 ) );
	} else if( ly == 0 ) {
		BUF( lx, ly - 1 ) = read_imagef( in, sampler, coord - ( int2 )( 0, 1 ) );
	}

    barrier( CLK_LOCAL_MEM_FENCE );

	dy = BUF( lx, ly + 1 ) - BUF( lx, ly - 1 );

	x1 = BUF( lx - 1, ly );
	x2 = BUF( lx, ly );
	x3 = BUF( lx + 1, ly );

	dx.x = x2.y - x1.w;
	dx.yz = x2.zw - x2.xy;
//	dx.y = x2.z - x2.x;
//	dx.z = x2.w - x2.y;
	dx.w = x3.x - x2.w;

    write_imagef( g2, coord, ( dx * dx + dy * dy ) );
    write_imagef( gx, coord, dx );
    write_imagef( gy, coord, dy );
}

