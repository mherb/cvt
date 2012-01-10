__kernel void pyrdown_binom3( __write_only image2d_t out,  __read_only image2d_t in, __local float4* buf, __local float4* buf2 )
{
    const sampler_t sampler = CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_CLAMP_TO_EDGE | CLK_FILTER_LINEAR;
	const int gx = get_global_id( 0 );
	const int gy = get_global_id( 1 );
	const int lx = get_local_id( 0 );
	const int ly = get_local_id( 1 );
    const int lw = get_local_size( 0 );
    const int lh = get_local_size( 1 );
	const int width = get_image_width( in );
	const int height = get_image_height( in );
	const int dstwidth = get_image_width( out );
	const int dstheight = get_image_height( out );
	const int basex = get_group_id( 0 ) * lw - 1;
	const int basey = get_group_id( 1 ) * lh - 1;
	float incx = ( float ) width / ( float ) dstwidth;
	float incy = ( float ) height / ( float ) dstheight;
	float2 coord;

	for( int y = ly; y < lh + 2; y += lh ) {
		for( int x = lx; x < lw + 2; x += lw ) {
			coord.x = ( basex + x + 0.5f ) * incx;
			coord.y = ( basey + y + 0.5f ) * incy;
			buf[ y * ( lw + 2 ) + x ] = read_imagef( in, sampler, coord );
		}
	}

	barrier( CLK_LOCAL_MEM_FENCE );

	for( int y = ly; y < lh + 2; y += lh ) {
		buf2[ y * lw + lx ] =  0.5f  * buf[ y * ( lw + 2 ) + lx + 1 ] +
							   0.25f * ( buf[ y * ( lw + 2 ) + lx + 0 ] + buf[ y * ( lw + 2 ) + lx + 2 ] );
	}

	barrier( CLK_LOCAL_MEM_FENCE );

	if( gx >= dstwidth || gy >= dstheight )
		return;


	float4 val =  0.5f  * buf2[ ( ly + 1 ) * lw + lx ] +
				  0.25f * ( buf2[ ( ly + 0 ) * lw + lx ] + buf2[ ( ly + 2 ) * lw + lx ] );

	write_imagef( out, ( int2 )( gx, gy ), val );

//	write_imagef( out, ( int2 )( gx, gy ), buf[ ( ly + 2 ) * ( lw + 4 ) + lx + 2 ] );
}

