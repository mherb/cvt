__kernel void simple( __write_only image2d_t out, float4 color )
{
    int2 coord;
    coord.x = get_global_id( 0 );
    coord.y = get_global_id( 1 );
    write_imagef( out, coord, color );
}

