__kernel void GRAD( __write_only image2d_t g2, __write_only image2d_t gx, __write_only image2d_t gy,  __read_only image2d_t in )
{
	const sampler_t sampler = CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_CLAMP_TO_EDGE | CLK_FILTER_NEAREST;
	int2 coord;
	float4 dx, dy, d2;
	float4 x1, x2, x3;

    coord.x = get_global_id( 0 );
    coord.y = get_global_id( 1 );

	dy = read_imagef( in, sampler, coord + ( int2 ) ( 0, 1 ) ) - read_imagef( in, sampler, coord - ( int2 ) ( 0, 1 ) );

	x1 = read_imagef( in, sampler, coord - ( int2 ) ( 1, 0 ) );
	x2 = read_imagef( in, sampler, coord );
	x3 = read_imagef( in, sampler, coord + ( int2 ) ( 1, 0 ) );

	dx.x = x2.y - x1.w;
	dx.yz = x2.zw - x2.xy;
//	dx.y = x2.z - x2.x;
//	dx.z = x2.w - x2.y;
	dx.w = x3.x - x2.w;

//	dx = -dx;
//	dy = -dy;

    write_imagef( g2, coord, ( dx * dx + dy * dy ) + ( float4 ) 1e-6f );
    write_imagef( gx, coord, dx );
    write_imagef( gy, coord, dy );
}

