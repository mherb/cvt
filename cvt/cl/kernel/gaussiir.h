#ifndef CL_gaussiir_H
#define CL_gaussiir_H
static const char _gaussiir_source[ ] =
"__kernel void gaussiir( __global float4* buffer, __read_only image2d_t input, const int w, const int h, const float4 n, const float4 m, const float4 d )\n" \
"{\n" \
"	const sampler_t sampler = CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_NONE | CLK_FILTER_NEAREST;\n" \
"	int2 coord;\n" \
"    float4 x[ 4 ];\n" \
"    float4 y[ 4 ];\n" \
"	float4 yn;\n" \
"\n" \
"	coord.x = 0;\n" \
"    coord.y = get_global_id( 0 );\n" \
"	buffer += get_global_id( 0 ) * w;\n" \
"\n" \
"	if( coord.y >= h )\n" \
"		return;\n" \
"\n" \
"	float b1 = ( n.s0 + n.s1 + n.s2 + n.s3 ) / ( d.s0 + d.s1 + d.s2 + d.s3 + 1.0f );\n" \
"	float b2 = ( m.s0 + m.s1 + m.s2 + m.s3 ) / ( d.s0 + d.s1 + d.s2 + d.s3 + 1.0f );\n" \
"\n" \
"	// forward pass\n" \
"	x[ 0 ] = read_imagef( input, sampler, coord );\n" \
"	x[ 1 ] = read_imagef( input, sampler, coord + ( int2 ) ( 1, 0 ) );\n" \
"	x[ 2 ] = read_imagef( input, sampler, coord + ( int2 ) ( 2, 0 ) );\n" \
"	x[ 3 ] = read_imagef( input, sampler, coord + ( int2 ) ( 3, 0 ) );\n" \
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
"	buffer[ 0 ] = y[ 0 ];\n" \
"	buffer[ 1 ] = y[ 1 ];\n" \
"	buffer[ 2 ] = y[ 2 ];\n" \
"	buffer[ 3 ] = y[ 3 ];\n" \
"\n" \
"    for( int i = 4; i < w; i++ ) {\n" \
"		coord.x = i;\n" \
"        x[ 0 ] = x[ 1 ]; x[ 1 ] = x[ 2 ]; x[ 2 ] = x[ 3 ];\n" \
"        x[ 3 ] = read_imagef( input, sampler, coord );\n" \
"        yn = n.s0 * x[ 3 ] + n.s1 * x[ 2 ] + n.s2 * x[ 1 ] + n.s3 * x[ 0 ]\n" \
"			 - d.s0 * y[ 3 ] - d.s1 * y[ 2 ] - d.s2 * y[ 1 ] - d.s3 * y[ 0 ];\n" \
"		buffer[ i ] = yn;\n" \
"		y[ 0 ] = y[ 1 ]; y[ 1 ] = y[ 2 ]; y[ 2 ] = y[ 3 ]; y[ 3 ]= yn;\n" \
"    }\n" \
"\n" \
"    // reverse pass\n" \
"	coord.x = w - 1;\n" \
"	x[ 0 ] = read_imagef( input, sampler, coord );\n" \
"	x[ 1 ] = read_imagef( input, sampler, coord - ( int2 ) ( 1, 0 ) );\n" \
"	x[ 2 ] = read_imagef( input, sampler, coord - ( int2 ) ( 2, 0 ) );\n" \
"	x[ 3 ] = read_imagef( input, sampler, coord - ( int2 ) ( 3, 0 ) );\n" \
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
"	buffer[ w - 1 ] += y[ 0 ];\n" \
"	buffer[ w - 2 ] += y[ 1 ];\n" \
"	buffer[ w - 3 ] += y[ 2 ];\n" \
"	buffer[ w - 4 ] += y[ 3 ];\n" \
"\n" \
"    for (int i = w-5; i >= 0; i--) {\n" \
"		coord.x = i;\n" \
"        x[ 0 ] = x[ 1 ]; x[ 1 ] = x[ 2 ]; x[ 2 ] = x[ 3 ];\n" \
"        x[ 3 ] = read_imagef( input, sampler, coord );\n" \
"		yn = m.s0 * x[ 3 ] + m.s1 * x[ 2 ] + m.s2 * x[ 1 ] + m.s3 * x[ 0 ]\n" \
"			 - d.s0 * y[ 3 ] - d.s1 * y[ 2 ] - d.s2 * y[ 1 ] - d.s3 * y[ 0 ];\n" \
"		buffer[ i ] += yn;\n" \
"		y[ 0 ] = y[ 1 ]; y[ 1 ] = y[ 2 ]; y[ 2 ] = y[ 3 ]; y[ 3 ]= yn;\n" \
"    }\n" \
"}\n" \
"\n";
#endif
