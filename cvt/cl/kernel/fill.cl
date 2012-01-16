__kernel void fill( __write_only image2d_t out, float4 value )
{
	int w = get_image_width( out );
	int h = get_image_height( out );

    int2 coord;
	coord.x = get_global_id( 0 );
	coord.y = get_global_id( 1 );

	if( coord.x >= w || coord.y >= h )
		return;

	write_imagef( out, coord, value );
}
