__kernel void prefixsum_pblock_outerrgb( __write_only image2d_t out_RR_RG_RB, __write_only image2d_t out_GG_GB_BB,  __read_only image2d_t in, __local float8* buf )
{
    const sampler_t sampler = CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_CLAMP | CLK_FILTER_NEAREST;
	const int lx = get_local_id( 0 );
	const int ly = get_local_id( 1 );
    const int lw = get_local_size( 0 );
//    const int lh = get_local_size( 1 );
	const int width = get_image_width( out_RR_RG_RB );
	const int height = get_image_height( out_RR_RG_RB );
	int2 coord;
	int lid = mul24( lw, ly ) + lx;
	float4 rgb;
	float8 tmp;

	coord.x = get_global_id( 0 );
	coord.y = get_global_id( 1 );

	rgb  = read_imagef( in, sampler, coord );
	tmp = ( float8 ) ( rgb.x * rgb.xyz, 0.0f, rgb.y * rgb.yz, rgb.z * rgb.z, 0.0f );
	buf[ lid ] = tmp;

	// assume lw == lh
	for( int offset = 1; offset < lw; offset <<= 1 )
	{
		barrier( CLK_LOCAL_MEM_FENCE );
		if( lx >= offset )
			tmp += buf[ lid - offset ];
		barrier( CLK_LOCAL_MEM_FENCE );
		buf[ lid ] = tmp;

		barrier( CLK_LOCAL_MEM_FENCE );
		if( ly >= offset )
			tmp += buf[ lid - mul24( lw, offset ) ];
		barrier( CLK_LOCAL_MEM_FENCE );
		buf[ lid ] = tmp;
	}

	if( coord.x < width && coord.y < height ) {
		write_imagef( out_RR_RG_RB, coord, tmp.lo );
		write_imagef( out_GG_GB_BB, coord, tmp.hi );
	}
}

