#define MAXDIFF 2.0f

__kernel void stereogcv_occlusioncheck( __write_only image2d_t out, __read_only image2d_t img0, __read_only image2d_t img1, const float dscale )
{
	const sampler_t sampler = CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_CLAMP | CLK_FILTER_LINEAR;
	int2 coord;
	const int width = get_image_width( img0 );
	const int height = get_image_height( img0 );
	float d0, d1, dout, diff;

	coord.x = get_global_id( 0 );
	coord.y = get_global_id( 1 );

	if( coord.x >= width || coord.y >= height )
		return;

	dout = read_imagef( img0, sampler, coord ).x;
	d0 = dout * dscale;
	d1 = read_imagef( img1, sampler, ( float2 ) ( - d0 + ( float ) coord.x, coord.y ) ).x * dscale;

	diff = fabs( d0 - d1 );
	if( diff <= MAXDIFF )
		write_imagef( out, coord, ( float4 ) dout );
	else
		write_imagef( out, coord, ( float4 ) 0 );
}
