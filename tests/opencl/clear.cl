__kernel void Denoise_CLEAR( __global __write_only float8* out )
{
out[ get_global_id( 0 ) ] = ( float8 ) 0.0f;
}
