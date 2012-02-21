#define MAXDIFF 2.0f

__kernel void stereogcv_occlusioncheck( __write_only image2d_t out, __read_only image2d_t img0, __read_only image2d_t img1, const float dscale )
{
	const sampler_t sampler = CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_CLAMP | CLK_FILTER_NEAREST;
	const sampler_t samplerlin = CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_CLAMP | CLK_FILTER_LINEAR;
	int2 coord;
	const int width = get_image_width( img0 );
	const int height = get_image_height( img0 );
	float d0, d1, din0, din1, diff;

	coord.x = get_global_id( 0 );
	coord.y = get_global_id( 1 );

	if( coord.x >= width || coord.y >= height )
		return;

	din0 = read_imagef( img0, sampler, coord ).x;
	d0 = din0 * dscale;
	din1 = read_imagef( img1, samplerlin, ( float2 ) ( - d0 + ( float ) coord.x + 0.5f, coord.y + 0.5f ) ).x;
	d1 = din1 * dscale;

	diff = fabs( - d0 + d1 );
	if( diff <= MAXDIFF )
		write_imagef( out, coord, ( float4 ) ( din0 + din1 ) * 0.5f );
	else
		write_imagef( out, coord, ( float4 ) 0 );
}
