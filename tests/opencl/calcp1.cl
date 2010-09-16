__kernel void Denoise_CALCP1( __write_only image2d_t pxout,  __write_only image2d_t pyout,
							  __read_only image2d_t pxin, __read_only image2d_t pyin,
							  __read_only image2d_t dst, const float taulambda, __local float4* buf )
{
	const sampler_t sampler = CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_CLAMP_TO_EDGE | CLK_FILTER_NEAREST;
	float4 p, px, py, pdx, pdy, norm;
	const int width = get_global_size( 0 );
	const int height = get_global_size( 1 );
	int2 coord;
    const int lx = get_local_id( 0 );
    const int ly = get_local_id( 1 );
    const int lw = get_local_size( 0 );
    const int lh = get_local_size( 1 );

	coord.x = get_global_id( 0 );
	coord.y = get_global_id( 1 );

	p = read_imagef( dst, sampler, coord );
#define BUF( x, y ) buf[ mul24(( y ),( lw + 1 )) + x ]
	BUF( lx, ly ) = p;

	if( lx == lw - 1 )
		BUF( lx + 1 , ly ) = read_imagef( dst, sampler, coord + ( int2 )( 1, 0 ) );
	if( ly == lh - 1 )
		BUF( lx, ly + 1 ) = read_imagef( dst, sampler, coord + ( int2 )( 0, 1 ) );

    barrier( CLK_LOCAL_MEM_FENCE );

	px = read_imagef( pxin, sampler, coord );
	py = read_imagef( pyin, sampler, coord );

	pdx.xy = p.zw - p.xy;
	if( coord.x != width - 1 )
		pdx.zw = BUF( lx + 1, ly ).xy - p.zw;
	else
		pdx.zw = ( float2 ) 0.0f;


	if( coord.y != height - 1 )
		pdy = BUF( lx, ly + 1 )  - p;
	else
		pdy = ( float4 ) 0.0f;

	px = px + taulambda * pdx;
	py = py + taulambda * pdy;

	norm = fmin( ( float4 ) 1.0f, rsqrt( px * px + py * py ) );
	norm = fmax( norm, ( float4 ) 1e-6f);
	px = px * norm;
	py = py * norm;

    write_imagef( pxout, coord, px );
    write_imagef( pyout, coord, py );
}

