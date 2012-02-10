__kernel void tvl1( __write_only image2d_t eout, __write_only image2d_t uout, __read_only image2d_t uimg, __read_only image2d_t u0img, __read_only image2d_t warp, __read_only image2d_t e1/*, __read_only image2d_t e0*/, const float lambda, const float theta/*, const float t*/, __local float4* buf, __local float4* buf2  )
{
    const sampler_t sampler = CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_CLAMP_TO_EDGE | CLK_FILTER_NEAREST;
	const int gx = get_global_id( 0 );
	const int gy = get_global_id( 1 );
	const int lx = get_local_id( 0 );
    const int ly = get_local_id( 1 );
    const int lw = get_local_size( 0 );
    const int lh = get_local_size( 1 );
    const int bstride = lw + 2;
	const int2 base  = ( int2 )( get_group_id( 0 ) * lw, get_group_id( 1 ) * lh );
	const int2 base2 = ( int2 )( get_group_id( 0 ) * lw - 1, get_group_id( 1 ) * lh - 1 );
	const float lamdatheta = lambda * theta;
	float4 w, pout;
	float u, u0, norm;
	float dx, dy;

	for( int y = ly; y < lh + 2; y += lh ) {
		for( int x = lx; x < lw + 2; x += lw ) {
			// px, py
			buf[ mul24( y, bstride ) + x ] = read_imagef( e1, sampler, base2 + ( int2 )( x, y ) );// * ( 1.0f + t ) - t * read_imagef( e0, samplerb, base2 + ( int2 )( x, y ) );
		}
	}

	barrier( CLK_LOCAL_MEM_FENCE );

#define BUF( x, y ) ( buf[ mul24( ( y ) + 1, bstride ) + ( x ) + 1 ] )
#define BUF2( x, y ) ( buf2[ mul24( ( y ), ( lw + 1 ) ) + ( x ) ] )

	for( int y = ly; y < lh + 1; y += lh ) {
		for( int x = lx; x < lw + 1; x += lw ) {
			// get I_t, I_x
			w = read_imagef( warp, sampler, base + ( int2 )( x, y ) );
			u = read_imagef( uimg, sampler, base + ( int2 )( x, y ) ).x;
			u0 = read_imagef( u0img, sampler, base + ( int2 )( x, y ) ).x;

			/* thresholding scheme */
			/* I_t + \nabla I \cdot u*/
			float dt = w.x + w.y * ( u - u0 );
			float ltg2 = lamdatheta * w.y * w.y;
			if( dt < - ltg2 )
				u = u + lamdatheta * w.y;
			else if( dt > ltg2 )
				u = u - lamdatheta * w.y;
			else if( fabs( dt ) <= ltg2 && fabs( w.y ) >= 1e-8f )
				u = u - ( dt / w.y );

			dx = BUF( x, y ).x - BUF( x - 1, y ).x;
			dy = BUF( x, y ).y - BUF( x, y - 1 ).y;
			// image + theta * div( p )
			BUF2( x, y ).x = u + theta * ( dx + dy );
			//BUF2( x, y ).xy = BUF2(x,y).xy*2-u;
			BUF2( x, y ).w  = w.w;
		}
	}

	barrier( CLK_LOCAL_MEM_FENCE );

	if( gx >= get_image_width( eout ) || gy >= get_image_height( eout ) )
		return;

	w = BUF2( lx, ly );
	dx = BUF2( lx + 1, ly ).x - w.x;
	dy = BUF2( lx, ly + 1 ).x - w.x;

	// HUBER
#define EPS 0.04f
	float4 delta = ( float4 ) ( dx, dy, 0, 0 );
	pout = BUF( lx, ly ) + ( 1.0f / ( 4.0f * theta ) ) * ( delta - EPS * BUF( lx, ly) );
//	float n = fmax(  1.0f,  fmax( fast_length( pout.xy ), fast_length( pout.zw ) ) / w.w );
//	float n = fmax(  1.0f,  fast_length( pout ) / w.w );
//	pout = pout / ( float4 ) ( n );

	norm = fmax( 1.0f, fast_length( pout.xy ) / w.w );
	pout = pout / norm;

	write_imagef( eout, ( int2 ) ( gx, gy ), pout );
	write_imagef( uout, ( int2 ) ( gx, gy ), ( float4 ) ( w.x, 0.0f, 0.0f, 0.0f ) );
}
