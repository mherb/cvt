__kernel void Denoise_CLEARIMG( __write_only image2d_t out )
{
    int2 coord;

    coord.x = get_global_id( 0 );
    coord.y = get_global_id( 1 );
    write_imagef( out, coord, ( float4 ) 0.0f );
}
