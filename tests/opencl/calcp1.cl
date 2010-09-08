__kernel void Denoise_CALCP1( __write_only image2d_t pxout,  __write_only image2d_t pyout,
							  __read_only image2d_t pxin, __read_only image2d_t pyin,
							  __read_only image2d_t dst, __read_only image2d_t src , const float taulambda )
{
	const sampler_t sampler = CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_CLAMP_TO_EDGE | CLK_FILTER_NEAREST;
	float4 p, px, py, pdx, pdy, norm;
	int2 coord;

	coord.x = get_global_id( 0 );
	coord.y = get_global_id( 1 );

	p = read_imagef( dst, sampler, coord );
	px = read_imagef( pxin, sampler, coord );
	py = read_imagef( pyin, sampler, coord );

//	if( coord.x != 0) {
		pdx = read_imagef( dst, sampler, coord + ( int2 )( 1, 0 ) ) - p;
//	} else {
//		pdx = ( float4 ) 0.0f;
//	}

//	if( coord.y != 0) {
		pdy = read_imagef( dst, sampler, coord + ( int2 )( 0, 1 ) ) - p;
//	} else {
//		pdy = ( float4 ) 0.0f;
//	}

	px = px - taulambda * pdx;
	py = py - taulambda * pdy;

	norm = fmin( ( float4 ) 1.0f, rsqrt( px * px + py * py ) );
	px = px * norm;
	py = py * norm;

    write_imagef( pxout, coord, px );
    write_imagef( pyout, coord, py );
}

