__kernel void fgp_clear( __write_only image2d_t out )
{
	int w = get_image_width( out );
	int h = get_image_height( out );

    int2 coord;
	coord.x = get_global_id( 0 );
	coord.y = get_global_id( 1 );

	if( coord.x < w && coord.y < h ) {
		write_imagef( out, coord, ( float4 ) 0.0f );
	}
}
