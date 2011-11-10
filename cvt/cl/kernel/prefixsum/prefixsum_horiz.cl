__kernel void prefixsum_horiz( __write_only image2d_t out,  __read_only image2d_t in, const int blocksize )
{
    const sampler_t sampler = CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_NONE | CLK_FILTER_NEAREST;
	const int width = get_image_width( out );
	const int height = get_image_height( out );
	int2 coord;
	float4 accum = ( float4 ) 0;

	coord.y = get_global_id( 0 );

	if( coord.y < height ) {
		for( coord.x = blocksize - 1; coord.x < width; coord.x += blocksize ) {
			accum += read_imagef( in, sampler, coord );
			write_imagef( out, coord, accum );
		}
//		coord.x = width - 1;
//		accum += read_imagef( in, sampler, coord );
//		write_imagef( out, coord, accum );
	}
}

