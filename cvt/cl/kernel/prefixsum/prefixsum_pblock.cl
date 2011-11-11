__kernel void prefixsum_pblock( __write_only image2d_t out,  __read_only image2d_t in, __local float4* buf )
{
    const sampler_t sampler = CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_CLAMP | CLK_FILTER_NEAREST;
	const int lx = get_local_id( 0 );
	const int ly = get_local_id( 1 );
    const int lw = get_local_size( 0 );
    const int lh = get_local_size( 1 );
	const int width = get_image_width( out );
	const int height = get_image_height( out );
	int2 coord;
	int lid = lw * ly + lx;

	coord.x = get_global_id( 0 );
	coord.y = get_global_id( 1 );

	buf[ lid ] = read_imagef( in, sampler, coord );

	barrier( CLK_LOCAL_MEM_FENCE );

	for( int offset = 1; offset < lw; offset <<= 1 )
	{
		if( lx >= offset) {
			buf[ lid ] += buf[ lid - offset ];
		}
		barrier( CLK_LOCAL_MEM_FENCE );
	}

	for( int offset = 1; offset < lh; offset <<= 1 )
	{
		if( ly >= offset) {
			buf[ lid ] += buf[ lw * ( ly - offset ) + lx ];
		}
		barrier( CLK_LOCAL_MEM_FENCE );
	}

	if( coord.x < width && coord.y < height )
		write_imagef( out, coord, buf[ lid ] );
}

