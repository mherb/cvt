__kernel void COPYIMG( __write_only image2d_t out, __read_only image2d_t in )
{
    const sampler_t sampler = CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_NONE | CLK_FILTER_NEAREST;
    int2 coord;
    float4 val;

    coord.x = get_global_id( 0 );
    coord.y = get_global_id( 1 );
    val = read_imagef( in, sampler, coord );
    write_imagef( out, coord, val );
}
