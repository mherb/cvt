__kernel void guidedfilter_calcab( __write_only image2d_t imga, __write_only image2d_t imgb,
								   __read_only image2d_t imgmeanG, __read_only image2d_t imgmeanS,
								   __read_only image2d_t imgmeanSG, __read_only image2d_t imgmeanGG, const float epsilon )
{
    const sampler_t sampler = CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_CLAMP | CLK_FILTER_NEAREST;
	const int width = get_image_width( imga );
	const int height = get_image_height( imga );
	int2 coord;
	float4 a, b, meanG, meanS, meanSG, meanGG, cov, var;

	coord.x = get_global_id( 0 );
	coord.y = get_global_id( 1 );

	meanG = read_imagef( imgmeanG, sampler, coord );
	meanS = read_imagef( imgmeanS, sampler, coord );
	meanSG = read_imagef( imgmeanSG, sampler, coord );
	meanGG = read_imagef( imgmeanGG, sampler, coord );

	cov = meanSG - meanG * meanS;
	var = meanGG - meanG * meanG;
	a = cov / ( var + ( float4 ) epsilon );
	b = meanS - a * meanG;

	if( coord.x < width && coord.y < height ) {
		write_imagef( imga, coord, a );
		write_imagef( imgb, coord, b );
	}
}

