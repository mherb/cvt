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
	int lid1 = lw * ly + lx;
	int lid2 = lid1 + mul24( lw, lh );

	coord.x = get_global_id( 0 );
	coord.y = get_global_id( 1 );

	buf[ lid2 ] = read_imagef( in, sampler, coord );

	barrier( CLK_LOCAL_MEM_FENCE );

	for( int offset = 1; offset < lw; offset <<= 1 )
	{
		int tmp = lid1;
		lid1 = lid2;
		lid2 = tmp;

		if( lx >= offset) {
			buf[ lid2 ] = buf[ lid1 ] + buf[ lid1 - offset ];
		} else {
			buf[ lid2 ] = buf[ lid1 ];
		}
		barrier( CLK_LOCAL_MEM_FENCE );
	}

	for( int offset = 1; offset < lh; offset <<= 1 )
	{
		int tmp = lid1;
		lid1 = lid2;
		lid2 = tmp;

		if( ly >= offset) {
			buf[ lid2 ] = buf[ lid1 ] + buf[ lid1 - mul24( lw, offset ) ];
		} else {
			buf[ lid2 ] = buf[ lid1 ];
		}

		barrier( CLK_LOCAL_MEM_FENCE );
	}

	if( coord.x < width && coord.y < height )
		write_imagef( out, coord, buf[ lid2 ] );
}

