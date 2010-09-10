__kernel void BILINDOWN( __write_only image2d_t out,  __read_only image2d_t in )
{
	const sampler_t sampler = CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_CLAMP_TO_EDGE | CLK_FILTER_LINEAR;
	int2 coord;
	float2 coordin;
    float4 v, in1, in2;

    coord.x = get_global_id( 0 );
    coord.y = get_global_id( 1 );
	coordin.x = ( float ) coord.x * 2.0f + 0.5f;
	coordin.y = ( float ) coord.y * 2.0f;
    in1 = read_imagef( in, sampler, coordin );
    in2 = read_imagef( in, sampler, coordin + ( float2 ) ( 1.0f, 0.0f ) );
	v = ( float4 ) ( 0.5f * ( in1.xz + in1.yw ), 0.5f * ( in2.xz + in2.yw ) );
    write_imagef( out, coord, v );
}
