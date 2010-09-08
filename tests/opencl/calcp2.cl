__kernel void Denoise_CALCP2( __write_only image2d_t dst,
							  __read_only image2d_t src,
							  __read_only image2d_t pxin, __read_only image2d_t pyin, const float lambda )
{
	const sampler_t sampler = CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_CLAMP_TO_EDGE | CLK_FILTER_NEAREST;
	int width = get_global_size( 0 );
	int height = get_global_size( 1 );
	float4 i, px, py, pdx, pdy;
	int2 coord;

	coord.x = get_global_id( 0 );
	coord.y = get_global_id( 1 );

	i = read_imagef( src, sampler, coord );
	px = read_imagef( pxin, sampler, coord );
	py = read_imagef( pyin, sampler, coord );

//	if( coord.x == width - 1 ) {
//		pdx = - read_imagef( pxin, sampler, coord - ( int2 ) ( 1, 0 ) );
//	} else if( coord.x == 0) {
//		pdx = px;
//	} else {
		pdx = px - read_imagef( pxin, sampler, coord - ( int2 ) ( 1, 0 ) );
//	}

//	if( coord.y == height - 1 ) {
//		pdy = - read_imagef( pyin, sampler, coord - ( int2 )( 0, 1 ) );
//	} else if( coord.y == 0) {
//		pdy = py;
//	} else {
		pdy = py - read_imagef( pyin, sampler, coord - ( int2 )( 0, 1 ) );
//	}

	i = i - lambda * ( pdx + pdy );
    write_imagef( dst, coord, i );
}


