#ifndef CL_gaussiir2_H
#define CL_gaussiir2_H
static const char _gaussiir2_source[ ] =
"__kernel void gaussiir2( __write_only image2d_t output, __global float4* buffer, __global float4* buffer2, const int w, const int h, const float4 n, const float4 m, const float4 d )\n" \
"{\n" \
"	int2 coord;\n" \
"    float4 x[ 4 ];\n" \
"    float4 y[ 4 ];\n" \
"	float4 xn, yn;\n" \
"	const int stride = w;\n" \
"\n" \
"    coord.x = get_global_id( 0 );\n" \
"	buffer += get_global_id( 0 );\n" \
"\n" \
"	if( coord.x >= w )\n" \
"		return;\n" \
"\n" \
"	float b1 = ( n.s0 + n.s1 + n.s2 + n.s3 ) / ( d.s0 + d.s1 + d.s2 + d.s3 + 1.0f );\n" \
"	float b2 = ( m.s0 + m.s1 + m.s2 + m.s3 ) / ( d.s0 + d.s1 + d.s2 + d.s3 + 1.0f );\n" \
"\n" \
"	// forward pass\n" \
"	x[ 0 ] = buffer[ 0 ];\n" \
"	x[ 1 ] = buffer[ stride ];\n" \
"	x[ 2 ] = buffer[ 2 * stride ];\n" \
"	x[ 3 ] = buffer[ 3 * stride ];\n" \
"\n" \
"	y[ 0 ] = n.s0 * x[ 0 ] + n.s1 * x[ 0 ] + n.s2 * x[ 0 ] + n.s3 * x[ 0 ]\n" \
"		- b1 * ( d.s0 * x[ 0 ] + d.s1 * x[ 0 ] + d.s2 * x[ 0 ] + d.s3 * x[ 0 ] );\n" \
"	y[ 1 ] = n.s0 * x[ 1 ] + n.s1 * x[ 0 ] + n.s2 * x[ 0 ] + n.s3 * x[ 0 ]\n" \
"		- d.s0 * y[ 0 ] - b1 * ( d.s1 * x[ 0 ] + d.s2 * x[ 0 ] + d.s3 * x[ 0 ] );\n" \
"	y[ 2 ] = n.s0 * x[ 2 ] + n.s1 * x[ 1 ] + n.s2 * x[ 0 ] + n.s3 * x[ 0 ]\n" \
"		- d.s0 * y[ 1 ] - d.s1 * y[ 0 ] - b1 * (  d.s2 * x[ 0 ] + d.s3 * x[ 0 ] );\n" \
"	y[ 3 ] = n.s0 * x[ 3 ] + n.s1 * x[ 2 ] + n.s2 * x[ 1 ] + n.s3 * x[ 0 ]\n" \
"		- d.s0 * y[ 2 ] - d.s1 * y[ 1 ] - d.s2 * y[ 0 ] - b1 * d.s3 * x[ 0 ];\n" \
"\n" \
"	buffer2[ 0 ] = y[ 0 ];\n" \
"	buffer2[ 1 ] = y[ 1 ];\n" \
"	buffer2[ 2 ] = y[ 2 ];\n" \
"	buffer2[ 3 ] = y[ 3 ];\n" \
"\n" \
"    for (int i = 4; i < h; i++) {\n" \
"        x[ 0 ] = x[ 1 ]; x[ 1 ] = x[ 2 ]; x[ 2 ] = x[ 3 ];\n" \
"        x[ 3 ] = buffer[ i * stride ];\n" \
"        yn = n.s0 * x[ 3 ] + n.s1 * x[ 2 ] + n.s2 * x[ 1 ] + n.s3 * x[ 0 ]\n" \
"			 - d.s0 * y[ 3 ] - d.s1 * y[ 2 ] - d.s2 * y[ 1 ] - d.s3 * y[ 0 ];\n" \
"		buffer2[ i ] = yn;\n" \
"		y[ 0 ] = y[ 1 ]; y[ 1 ] = y[ 2 ]; y[ 2 ] = y[ 3 ]; y[ 3 ]= yn;\n" \
"    }\n" \
"\n" \
"\n" \
"	mem_fence( CLK_GLOBAL_MEM_FENCE );\n" \
"\n" \
"    // reverse pass\n" \
"	coord.y = h - 1;\n" \
"	x[ 0 ] = buffer[ ( h - 1 ) * stride ];\n" \
"	x[ 1 ] = buffer[ ( h - 2 ) * stride ];\n" \
"	x[ 2 ] = buffer[ ( h - 3 ) * stride ];\n" \
"	x[ 3 ] = buffer[ ( h - 4 ) * stride ];\n" \
"\n" \
"	y[ 0 ] = m.s0 * x[ 0 ] + m.s1 * x[ 0 ] + m.s2 * x[ 0 ] + m.s3 * x[ 0 ]\n" \
"		- b2 * ( d.s0 * x[ 0 ] + d.s1 * x[ 0 ] + d.s2 * x[ 0 ] + d.s3 * x[ 0 ] );\n" \
"	y[ 1 ] = m.s0 * x[ 1 ] + m.s1 * x[ 0 ] + m.s2 * x[ 0 ] + m.s3 * x[ 0 ]\n" \
"		- d.s0 * y[ 0 ] - b2 * ( d.s1 * x[ 0 ] + d.s2 * x[ 0 ] + d.s3 * x[ 0 ] );\n" \
"	y[ 2 ] = m.s0 * x[ 2 ] + m.s1 * x[ 1 ] + m.s2 * x[ 0 ] + m.s3 * x[ 0 ]\n" \
"		- d.s0 * y[ 1 ] - d.s1 * y[ 0 ] - b2 * (  d.s2 * x[ 0 ] + d.s3 * x[ 0 ] );\n" \
"	y[ 3 ] = m.s0 * x[ 3 ] + m.s1 * x[ 2 ] + m.s2 * x[ 1 ] + m.s3 * x[ 0 ]\n" \
"		- d.s0 * y[ 2 ] - d.s1 * y[ 1 ] - d.s2 * y[ 0 ] - b2 * d.s3 * x[ 0 ];\n" \
"\n" \
"	write_imagef( output, coord,  buffer2[ h - 1 ] + y[ 0 ] );\n" \
"	write_imagef( output, coord - ( int2 ) ( 0, 1 ),  buffer2[ h - 2 ] + y[ 1 ] );\n" \
"	write_imagef( output, coord - ( int2 ) ( 0, 2 ),  buffer2[ h - 3 ] + y[ 2 ] );\n" \
"	write_imagef( output, coord - ( int2 ) ( 0, 3 ),  buffer2[ h - 4 ] + y[ 3 ] );\n" \
"\n" \
"    for (int i = h-5; i >= 0; i--) {\n" \
"        x[ 0 ] = x[ 1 ]; x[ 1 ] = x[ 2 ]; x[ 2 ] = x[ 3 ];\n" \
"        x[ 3 ] = buffer[ i * stride ];\n" \
"		yn = m.s0 * x[ 3 ] + m.s1 * x[ 2 ] + m.s2 * x[ 1 ] + m.s3 * x[ 0 ]\n" \
"			 - d.s0 * y[ 3 ] - d.s1 * y[ 2 ] - d.s2 * y[ 1 ] - d.s3 * y[ 0 ];\n" \
"		coord.y = i;\n" \
"		write_imagef( output, coord, buffer2[ i ] + yn );\n" \
"		y[ 0 ] = y[ 1 ]; y[ 1 ] = y[ 2 ]; y[ 2 ] = y[ 3 ]; y[ 3 ]= yn;\n" \
"    }\n" \
"}\n" \
"\n";
#endif
