__kernel void guidedfilter_applyab_gc_outer( __write_only image2d_t imgout,  __read_only image2d_t imgin, __read_only image2d_t imga, __read_only image2d_t imgb )
{
    const sampler_t sampler = CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_CLAMP | CLK_FILTER_NEAREST;
	const int width = get_image_width( imgout );
	const int height = get_image_height( imgout );
	int2 coord;
	float4 in, a, b, out;

	coord.x = get_global_id( 0 );
	coord.y = get_global_id( 1 );

	in = read_imagef( imgin, sampler, coord );
	a = read_imagef( imga, sampler, coord );
	b = read_imagef( imgb, sampler, coord );

	out = ( float4 ) dot( a, in ) + b;
	out.w = 1.0f;

	if( coord.x < width && coord.y < height )
		write_imagef( imgout, coord, out );
}

