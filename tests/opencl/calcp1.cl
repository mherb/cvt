__kernel void Denoise_CALCP1( __write_only image2d_t pxout,  __write_only image2d_t pyout,
							  __read_only image2d_t pxin, __read_only image2d_t pyin,
							  __read_only image2d_t dst, const float taulambda )
{
	const sampler_t sampler = CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_NONE | CLK_FILTER_NEAREST;
	float4 p, px, py, pdx, pdy, norm;
	int width = get_global_size( 0 );
	int height = get_global_size( 1 );
	int2 coord;

	coord.x = get_global_id( 0 );
	coord.y = get_global_id( 1 );

	p = read_imagef( dst, sampler, coord );
	px = read_imagef( pxin, sampler, coord );
	py = read_imagef( pyin, sampler, coord );

	pdx.xy = p.zw - p.xy;
	if( coord.x != width - 1 )
		pdx.zw = read_imagef( dst, sampler, coord + ( int2 )( 1, 0 ) ).xy - p.zw;
	else
		pdx.zw = ( float2 ) 0.0f;


	if( coord.y != height - 1 )
		pdy = read_imagef( dst, sampler, coord + ( int2 )( 0, 1 ) ) - p;
	else
		pdy = ( float4 ) 0.0f;

	px = px - taulambda * pdx;
	py = py - taulambda * pdy;

	norm = fmin( ( float4 ) 1.0f, native_rsqrt( px * px + py * py ) );
	px = px * norm;
	py = py * norm;

    write_imagef( pxout, coord, px );
    write_imagef( pyout, coord, py );
}

