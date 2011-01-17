__kernel void BC( __write_only image2d_t out,  __read_only image2d_t in, float brightness, float contrast )
{
	const sampler_t sampler = CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_NONE | CLK_FILTER_NEAREST;
	float4 pixel;
	int2 coord;

	coord.x = get_global_id( 0 );
	coord.y = get_global_id( 1 );
	pixel = read_imagef( in, sampler, coord );
//	pixel.xyz -= 0.5f;
//	pixel.xyz *= ( 1.0f + contrast );
//	pixel.xyz += 0.5f;
//	pixel.xyz *= contrast;
	float a = pixel.w;
	pixel *=  ( 1.0f + contrast );
	pixel +=  brightness;
	pixel.w = a;
	write_imagef( out, coord, pixel );
}


