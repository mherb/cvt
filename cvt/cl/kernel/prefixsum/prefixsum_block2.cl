__kernel void prefixsum_block2( __write_only image2d_t out,  __read_only image2d_t in, __local float4* bufh, __local float4* bufv )
{
    const sampler_t sampler = CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_CLAMP | CLK_FILTER_NEAREST;
	const int lx = get_local_id( 0 );
	const int ly = get_local_id( 1 );
	const int lw = get_local_size( 0 );
	const int lh = get_local_size( 1 );
	const int width = get_image_width( out );
	const int height = get_image_height( out );
	int2 coord;
	float4 tmp;
	local float4 s;

	coord.x = get_global_id( 0 );
	coord.y = get_global_id( 1 );

	if( lx == 0 && ly == 0 )
		s = read_imagef( in, sampler, coord - ( int2 ) ( 1, 1 ) );

	if( ly == 0 )
		bufv[ lx ] = read_imagef( in, sampler, coord - ( int2 ) ( 0, 1 ) );

	if( lx == 0 )
		bufh[ ly ] = read_imagef( in, sampler, coord - ( int2 ) ( 1, 0 ) );

	barrier( CLK_LOCAL_MEM_FENCE );

	if( !( coord.x < width && coord.y < height ) )
		return;

	if( lx < lw - 1 && ly < lh - 1 ) {
		tmp = read_imagef( in, sampler, coord );
		tmp += bufv[ lx ];
		tmp += bufh[ ly ];
		tmp += s;
		write_imagef( out, coord, tmp );
	}

	if( ly == 0 && coord.y && lx < lw - 1 )
		write_imagef( out, coord - ( int2 ) ( 0, 1 ), bufv[ lx ] + s );

	if( lx == 0 && coord.x && ly < lh - 1 )
		write_imagef( out, coord - ( int2 ) ( 1, 0 ), bufh[ ly ] + s );

	/* just for last lines exactly at the edge of the local size patch*/
	if( coord.y == height - 1 && coord.x < width && lx < lw - 1 && ly == lh - 1 ) {
		tmp = read_imagef( in, sampler, coord );
		write_imagef( out, coord, tmp + bufh[ ly ] );
	}

	if( coord.x == width - 1 && coord.y < height && ly < lh - 1 && lx == lw - 1 ) {
		tmp = read_imagef( in, sampler, coord );
		write_imagef( out, coord, tmp + bufv[ lx ] );
	}

}

