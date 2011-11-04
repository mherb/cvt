__kernel void CALCN( __write_only image2d_t out, __read_only image2d_t in1, __read_only image2d_t in2, const float t )
{
    const sampler_t sampler = CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_NONE | CLK_FILTER_NEAREST;
    int2 coord;
    float4 val1, val2;

    coord.x = get_global_id( 0 );
    coord.y = get_global_id( 1 );
    val1 = read_imagef( in1, sampler, coord );
    val2 = read_imagef( in2, sampler, coord );
    write_imagef( out, coord, val1 + t * ( val1 - val2 ) );
}
