__kernel void tvl1( __write_only image2d_t eout, __write_only image2d_t uout, __read_only image2d_t uimg, __read_only image2d_t u0img, __read_only image2d_t warp, __read_only image2d_t e1/*, __read_only image2d_t e0*/, const float lambda, const float theta/*, const float t*/, __local float4* buf, __local float4* buf2  )
{
    const sampler_t sampler = CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_CLAMP_TO_EDGE | CLK_FILTER_NEAREST;
    const sampler_t samplerb = CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_CLAMP | CLK_FILTER_NEAREST;
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
	float2 dx, dy, u, u0, norm;

	for( int y = ly; y < lh + 2; y += lh ) {
		for( int x = lx; x < lw + 2; x += lw ) {
			// px, py
			buf[ mul24( y, bstride ) + x ] = read_imagef( e1, samplerb, base2 + ( int2 )( x, y ) );// * ( 1.0f + t ) - t * read_imagef( e0, samplerb, base2 + ( int2 )( x, y ) );
		}
	}

	barrier( CLK_LOCAL_MEM_FENCE );

#define BUF( x, y ) ( buf[ mul24( ( y ) + 1, bstride ) + ( x ) + 1 ] )
#define BUF2( x, y ) ( buf2[ mul24( ( y ), ( lw + 1 ) ) + ( x ) ] )

	for( int y = ly; y < lh + 1; y += lh ) {
		for( int x = lx; x < lw + 1; x += lw ) {
			// get I_t, I_x, I_y
			w = read_imagef( warp, sampler, base + ( int2 )( x, y ) );
			u = read_imagef( uimg, sampler, base + ( int2 )( x, y ) ).xy;
			u0 = read_imagef( u0img, sampler, base + ( int2 )( x, y ) ).xy;

			/* thresholding scheme */
			/* I_t + \nabla I \cdot u*/
			float dt = w.x + dot( w.yz, u - u0 );
			float ltg2 = lamdatheta * dot( w.yz, w.yz );
			if( dt < - ltg2 )
				u = u + lamdatheta * w.yz;
			else if( dt > ltg2 )
				u = u - lamdatheta * w.yz;
			else
				u = u - ( dt / fmax( dot( w.yz, w.yz ), 1e-4f ) ) * w.yz;

			dx = BUF( x, y ).xz - BUF( x - 1, y ).xz;
			dy = BUF( x, y ).yw - BUF( x, y - 1 ).yw;
			// image + theta * div( p )
			BUF2( x, y ).xy = u + theta * ( ( float2 ) ( dx.x + dy.x, dx.y + dy.y ) );
			BUF2( x, y ).w  = w.w;
		}
	}

	barrier( CLK_LOCAL_MEM_FENCE );

	if( gx >= get_image_width( eout ) || gy >= get_image_height( eout ) )
		return;

	w = BUF2( lx, ly );
	dx = BUF2( lx + 1, ly ).xy - w.xy;
	dy = BUF2( lx, ly + 1 ).xy - w.xy;

	pout = BUF( lx, ly ) + ( 1.0f / ( 8.0f * theta ) ) * ( float4 ) ( dx.x, dy.x, dx.y, dy.y );
	norm = fmax( ( float2 ) 1.0f, ( float2 ) ( length( pout.xy )/ w.w, length( pout.zw ) ) / w.w );
	pout = pout / norm.xxyy;

	write_imagef( eout, ( int2 ) ( gx, gy ), pout );
	write_imagef( uout, ( int2 ) ( gx, gy ), ( float4 ) ( w.x, w.y, 0.0f, 0.0f ) );
}
