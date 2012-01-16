__kernel void stereogcv_costdepthconv( __write_only image2d_t out, __read_only image2d_t cd, float scale, float add )
{
	const sampler_t sampler = CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_NONE | CLK_FILTER_LINEAR;
	int w = get_image_width( out );
	int h = get_image_height( out );

    int2 coord;
	coord.x = get_global_id( 0 );
	coord.y = get_global_id( 1 );

	if( coord.x >= w || coord.y >= h )
		return;

	float4 value = read_imagef( cd, sampler, coord );
	write_imagef( out, coord, ( float4 ) ( value.y * scale + add ) );
}
