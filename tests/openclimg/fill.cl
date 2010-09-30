__kernel void fill( __write_only image2d_t output )
{
	int2 coord;
	float4 value = ( float4 ) ( 0.0f, 0.0f, 0.0f, 1.0f );
	int maxx = get_global_size( 0 );
	int maxy = get_global_size( 0 );
    coord.x = get_global_id( 0 );
    coord.y = get_global_id( 1 );

	value.y = ( float ) coord.x / ( float ) maxx;
	value.x = ( float ) coord.y / ( float ) maxy;
	write_imagef( output, coord, value );
}
