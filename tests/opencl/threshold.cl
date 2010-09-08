__kernel void Denoise_TH( __write_only image2d_t outimg, __read_only image2d_t inimg,
			  __read_only image2d_t px,  __read_only image2d_t py,
			  __read_only image2d_t origimg, __local float8* bufin, const float lambda, const float thetalambda )
{
    const sampler_t sampler = CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_NONE | CLK_FILTER_NEAREST;
    const sampler_t sampleredge = CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_CLAMP_TO_EDGE | CLK_FILTER_NEAREST;
    const int lstride = get_local_size( 0 ) + 1;
    const int lx = get_local_id( 0 );
    const int ly = get_local_id( 1 );
    int2 coord;
    float4 u, pdx, pdy, f, out, duf;
    float8 pval, ptmp;
    int4 cmp;
    float len;

    coord.x = get_global_id( 0 );
    coord.y = get_global_id( 1 );

    pval.lo = read_imagef( px, sampleredge, coord );
    pval.hi = read_imagef( py, sampleredge, coord );
    bufin[ lstride * ( ly + 1 ) + lx + 1 ] = pval;

    if( lx == 0 ) {
	ptmp.lo = read_imagef( px, sampleredge, coord - ( int2 ) ( 1, 0 ) );
	ptmp.hi = read_imagef( py, sampleredge, coord - ( int2 ) ( 0, 1 ) );
	bufin[ lstride * ( ly + 1 ) ] = ptmp;
    }
    if( ly == 0 ) {
	ptmp.lo = read_imagef( px, sampleredge, coord - ( int2 ) ( 1, 0 ) );
	ptmp.hi = read_imagef( py, sampleredge, coord - ( int2 ) ( 0, 1 ) );
	bufin[ lx + 1 ] = ptmp;
    }

    barrier( CLK_LOCAL_MEM_FENCE );

    u = read_imagef( inimg, sampler, coord );
    f = read_imagef( origimg, sampler, coord );

    pdx = pval.lo - bufin[ lstride * ( ly + 1 ) + lx ].lo;
    pdy = pval.hi - bufin[ lstride * ( ly ) + lx + 1 ].hi;

    u = u + lambda * ( pdx + pdy );
    duf = u - f;
    cmp = isgreater( duf, ( float4 ) thetalambda );
    out = select( f, u - ( float4 ) thetalambda, cmp );
    cmp = isless( duf, ( float4 ) -thetalambda );
    out = select( out, u + ( float4 ) thetalambda, cmp );
    write_imagef( outimg, coord, out );
}

