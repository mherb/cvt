__kernel void tvl1_dataadd( __write_only image2d_t udst, __read_only image2d_t uimg, __read_only image2d_t uimgorig, __read_only image2d_t warp, __read_only image2d_t e1, __read_only image2d_t e0, const float lambda, const float theta, const float t, __local float4* buf  )
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
	float2 dx, dy, u, uorig, norm;

	for( int y = ly; y < lh + 2; y += lh ) {
		for( int x = lx; x < lw + 2; x += lw ) {
			// px, py
			buf[ mul24( y, bstride ) + x ] = read_imagef( e1, samplerb, base2 + ( int2 )( x, y ) ) * ( 1.0f + t ) - t * read_imagef( e0, samplerb, base2 + ( int2 )( x, y ) );
		}
	}

	barrier( CLK_LOCAL_MEM_FENCE );

#define BUF( x, y ) ( buf[ mul24( ( y ) + 1, bstride ) + ( x ) + 1 ] )

	if( gx >= get_image_width( uimg ) || gy >= get_image_height( uimg ) )
		return;


	// get I_t, I_x, I_y
	w = read_imagef( warp, sampler, ( int2 )( gx, gy ) );
	u = read_imagef( uimg, sampler, ( int2 )( gx, gy ) ).xy;
	uorig = read_imagef( uimgorig, sampler, ( int2 )( gx, gy ) ).xy;

	/* thresholding scheme */
	/* I_t + \nabla I \cdot u */
	float dt = w.x + dot( w.yz, u );
	float ltg2 = lamdatheta * dot( w.yz, w.yz );
	if( dt < - ltg2 )
		u = u + lamdatheta * w.yz;
	else if( dt > ltg2 )
		u = u - lamdatheta * w.yz;
	else
		u = u - ( dt / fmax( dot( w.yz, w.yz ), 1e-6f ) ) * w.yz;

	dx = BUF( lx, ly ).xz - BUF( lx - 1, ly ).xz;
	dy = BUF( lx, ly ).yw - BUF( lx, ly - 1 ).yw;
	/* image + theta * div( p ) */
	uorig += u + theta * ( ( float2 ) ( dx.x + dy.x, dx.y + dy.y ) );
	write_imagef( udst, ( int2 )( gx,gy ), ( float4 ) ( uorig.x, uorig.y, 0.0f, 0.0f ) );
}
