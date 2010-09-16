__kernel void BILINUP( __write_only image2d_t out,  __read_only image2d_t in, const float mul )
{
	const sampler_t sampler = CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_CLAMP_TO_EDGE | CLK_FILTER_LINEAR;
	float2 coord;
	int2 coordout;
    float4 out1, out2, in1, in2, in3;

	coordout.x = get_global_id( 0 );
	coordout.y = get_global_id( 1 );
    coord.x = ( float ) coordout.x + 0.5f;
    coord.y = ( float ) coordout.y + 0.25f;
	coordout = coordout * 2;

    in1 = read_imagef( in, sampler, coord - ( float2 ) ( 1.0f, 0.0f ));
    in2 = read_imagef( in, sampler, coord );
    in3 = read_imagef( in, sampler, coord + ( float2 ) ( 1.0f, 0.0f ) );

	out1.xy = mix( in1.zw, in2.xy , 0.75f );
	out1.zw = mix( in2.xy, in2.zw , 0.25f );
	out2.xy = mix( in2.xy, in2.zw , 0.75f );
	out2.zw = mix( in2.zw, in3.xy , 0.25f );

	out1 = out1 * mul;
	out2 = out2 * mul;

	write_imagef( out, coordout, out1 );
	write_imagef( out, coordout + ( int2 ) ( 1, 0 ), out2 );

	coord.y = coord.y + 0.5f;
    in1 = read_imagef( in, sampler, coord - ( float2 ) ( 1.0f, 0.0f ));
    in2 = read_imagef( in, sampler, coord );
    in3 = read_imagef( in, sampler, coord + ( float2 ) ( 1.0f, 0.0f ) );

	out1.xy = mix( in1.zw, in2.xy , 0.75f );
	out1.zw = mix( in2.xy, in2.zw , 0.25f );
	out2.xy = mix( in2.xy, in2.zw , 0.75f );
	out2.zw = mix( in2.zw, in3.xy , 0.25f );

	out1 = out1 * mul;
	out2 = out2 * mul;

	coordout.y = coordout.y + 1;
	write_imagef( out, coordout, out1 );
	write_imagef( out, coordout + ( int2 ) ( 1, 0 ), out2 );
}
