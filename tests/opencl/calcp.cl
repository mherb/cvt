__kernel void Denoise_CALCP( __write_only image2d_t outx, __write_only image2d_t outy,
			     __read_only image2d_t inx, __read_only image2d_t iny,
			    __read_only image2d_t img,
			    __local float8* plocal,
			    const float lambda, const float taulambda )
{
    const sampler_t sampler = CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_NONE | CLK_FILTER_NEAREST;
    const sampler_t sampleredge = CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_CLAMP_TO_EDGE | CLK_FILTER_NEAREST;
    const int x  = get_global_id( 0 );
    const int y  = get_global_id( 1 );
    const int lx = get_local_id( 0 );
    const int ly = get_local_id( 1 );
    const int lwidth  = get_local_size( 0 );
    const int lstride  = lwidth + 2;
    const int lheight = get_local_size( 1 );
    float4 pixel, pdx, pdy, pdxb, pdyb, pixelb, ptmp, norm;
    float8 p, pb;

    /* Fetch pixels from in corresponding to the old p */
    p.lo = read_imagef( inx, sampleredge, ( int2 ) ( x, y) );
    p.hi = read_imagef( iny, sampleredge, ( int2 ) ( x, y) );
    plocal[ lstride * ( ly + 1 ) + lx + 1 ] = p;

    /* Also fetch pixels from the left of the left border with edge clamping */
    if( lx == 0 ) {
	pb.lo = read_imagef( inx, sampleredge, ( int2 ) ( x - 1, y) );
	pb.hi = read_imagef( iny, sampleredge, ( int2 ) ( x - 1, y) );
	plocal[ lstride * ( ly + 1 ) + lx ] = pb;
    } else if( lx == lwidth - 1 ) {   /* Also fetch pixels from the right of the right border with edge clamping */
	pb.lo = read_imagef( inx, sampleredge, ( int2 ) ( x + 1, y) );
	pb.hi = read_imagef( iny, sampleredge, ( int2 ) ( x + 1, y) );
	plocal[ lstride * ( ly + 1 ) + lx + 2 ] = pb;
    }

    /* Also fetch pixels above the top of the upper border with edge clamping */
    if( ly == 0 ) {
	pb.lo = read_imagef( inx, sampleredge, ( int2 ) ( x, y - 1) );
	pb.hi = read_imagef( iny, sampleredge, ( int2 ) ( x, y - 1) );
	plocal[ lx + 1 ] = pb;
    } else if( ly == lheight - 1 ) { /* Also fetch pixels below the bottom of the lower border with edge clamping */
	pb.lo = read_imagef( inx, sampleredge, ( int2 ) ( x, y + 1) );
	pb.hi = read_imagef( iny, sampleredge, ( int2 ) ( x, y + 1) );
	plocal[ lstride * ( lheight + 1 ) + lx + 1 ] = pb;
    }

    barrier( CLK_LOCAL_MEM_FENCE );

    /* Fetch pixel value from image img */

    pixel = read_imagef( img, sampler, ( int2 ) ( x, y) );

    pdx = p.lo - plocal[ lstride * ( ly + 1 ) + lx ].lo;
    pdy = p.hi - plocal[ lstride * ( ly ) + lx + 1 ].hi;

    ptmp = pixel;
    pixel = ptmp + lambda * ( pdx + pdy );

    plocal[ lstride * ( ly + 1 ) + lx + 1 ].hi = pixel;

    if( lx == lwidth - 1 ) {
	    pb = plocal[ lstride * ( ly + 1 ) + lx + 2 ];
	    pixelb = read_imagef( img, sampler, ( int2 ) ( x + 1, y ) );
	    pdxb = pb.lo - p.lo;
	    pdyb = pb.hi - p.hi;
	    pixelb = pixelb + lambda * ( pdxb + pdyb );
	    plocal[ lstride * ( ly + 1 ) + lx + 2 ].hi = pixelb;
    }
    if( ly == lheight - 1 ) {
	    pb = plocal[ lstride * ( ly + 2 ) + lx + 1 ];
	    pixelb = read_imagef( img, sampler, ( int2 ) ( x, y + 1 ) );
	    pdxb = pb.lo - p.lo;
	    pdyb = pb.hi - p.hi;
	    pixelb = pixelb + lambda * ( pdxb + pdyb );
	    plocal[ lstride * ( ly + 2 ) + lx + 1 ].hi = pixelb;
    }

    barrier( CLK_LOCAL_MEM_FENCE );

    pdx = plocal[ lstride * ( ly + 1 ) + lx + 2 ].hi - pixel;
    pdy = plocal[ lstride * ( ly + 2 ) + lx + 1 ].hi - pixel;

    p = p + taulambda * ( float8 ) ( pdx, pdy );
    norm = fmin( ( float4 ) 1.0f, native_rsqrt( p.lo * p.lo + p.hi * p.hi ) );
    p = p * ( float8 ) ( norm, norm );
    write_imagef( outx, ( int2 ) ( x, y ), p.lo );
    write_imagef( outy, ( int2 ) ( x, y ), p.hi );
}

