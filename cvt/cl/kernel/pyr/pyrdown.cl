__kernel void pyrdown( __write_only image2d_t out,  __read_only image2d_t in )
{
    const sampler_t sampler = CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_CLAMP_TO_EDGE | CLK_FILTER_LINEAR;
	const int gx = get_global_id( 0 );
	const int gy = get_global_id( 1 );
	const int width = get_image_width( in );
	const int height = get_image_height( in );
	const int dstwidth = get_image_width( out );
	const int dstheight = get_image_height( out );
	float incx = ( float ) width / ( float ) dstwidth;
	float incy = ( float ) height / ( float ) dstheight;
	float2 coord;

	if( gx >= dstwidth || gy >= dstheight )
		return;

	coord.x = incx * ( gx + 0.5f );
	coord.y = incy * ( gy + 0.5f );

	float4 val = read_imagef( in, sampler, coord );
	write_imagef( out, ( int2 )( gx, gy ), val );
}

