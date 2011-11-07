__kernel void fgp_calcn( __write_only image2d_t out, __read_only image2d_t in1, __read_only image2d_t in2, const float t )
{
    const sampler_t sampler = CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_NONE | CLK_FILTER_NEAREST;
    int2 coord;
    float4 val1, val2;
	const int width = get_image_width( out );
	const int height = get_image_height( out );

    coord.x = get_global_id( 0 );
    coord.y = get_global_id( 1 );

	if( coord.x >= width || coord.y >= height )
		return;

    val1 = read_imagef( in1, sampler, coord );
    val2 = read_imagef( in2, sampler, coord );
    write_imagef( out, coord, val1 + t * ( val1 - val2 ) );
}
