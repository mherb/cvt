__kernel void clearbuffer_f( global float* out, int size )
{
	int id = get_global_id( 0 );

	if( id >= size )
		return;

	out[ id ] = 0.0f;
}
