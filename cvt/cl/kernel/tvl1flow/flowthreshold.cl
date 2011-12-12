__kernel void flow_threshold( __write_only image2d_t out, __read_only image2d_t u, __read_only image2d_t src1, __read_only image2d_t src2 , const float lambda, __local float4* buf1, __local float4* buf2  )
{
    const sampler_t samplerlin = CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_CLAMP_TO_EDGE | CLK_FILTER_LINEAR;
    const sampler_t sampler = CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_CLAMP_TO_EDGE | CLK_FILTER_NEAREST;
	const int gx = get_global_id( 0 );
	const int gy = get_global_id( 1 );
	const int lx = get_local_id( 0 );
	const int ly = get_local_id( 1 );
    const int lw = get_local_size( 0 );
    const int lh = get_local_size( 1 );
	const int width = get_image_width( u );
	const int height = get_image_height( u );
	const int2 base = ( int2 )( get_group_id( 0 ) * lw - 1, get_group_id( 1 ) * lh - 1 );
	float2 coord, delta;
	float dx2, dy2, dt, d2;

	for( int y = ly; y < lh + 2; y += lh ) {
		for( int x = lx; x < lw + 2; x += lw ) {
			buf[ y * ( lw + 2 ) + x ] = read_imagef( src1, sampler, base + ( int2 )( x, y ) );
		}
	}

	barrier( CLK_LOCAL_MEM_FENCE );

	if( gx >= dstwidth || gy >= dstheight )
		return;

	coord = ( float2 )( gx,gy ) + read_imagef( u, sampler, ( int2 )( gx, gy ) ).xy + ( float2 ) 0.5f;
	buf2[ y * ( lw ) + x ] = read_imagef( src2, samplerlin, coord );
	dx2 = read_imagef( src2, samplerlin, coord - ( float2 )( 1.0f, 0.0f ) ) - read_imagef( src2, samplerlin, coord + ( float2 )( 1.0f, 0.0f ) );
	dy2 = read_imagef( src2, samplerlin, coord - ( float2 )( 0.0f, 1.0f ) ) - read_imagef( src2, samplerlin, coord + ( float2 )( 0.0f, 1.0f ) );

// #define CSELECT( val ) dot( val, ( float4 )( 0.3333f, 0.3333f, 0.3333f, 0.0f ) )

 #define CSELECT( val ) ( ( val ).x )

	dt	     = CSELECT( buf2[ ( ly + 1 ) * ( lw + 2 ) + ( lx + 1 ) ] - buf1[ ( ly + 1 ) * ( lw + 2 ) + ( lx + 1 ) ] );
	delta.x  = CSELECT( 0.5f * ( dx2 + buf[ ( ly + 1 ) * ( lw + 2 ) + ( lx )  ] - buf[ ( ly + 1 ) * ( lw + 2 ) + ( lx + 2 ) ] ) );
	delta.y  = CSELECT( 0.5f * ( dy2 + buf[ ( ly  ) * ( lw + 2 ) + ( lx + 1 ) ] - buf[ ( ly + 2 ) * ( lw + 2 ) + ( lx + 1 ) ] ) );

	d2 = max( 1e-06f, fast_length( delta ) );

	if( dt < - lambda * d2 ) {
		coord += lambda * delta;
	} else if( dt > lambda * d2 ) {
		coord -= lambda * delta;
	} else {
		coord -= dt * ( delta / d2 );
	}

	write_imagef( out,( int2 )( gx, gy ), ( float4 ) coord  );
}
