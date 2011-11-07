#ifndef CL_fgp_clear_H
#define CL_fgp_clear_H
static const char _fgp_clear_source[ ] =
"__kernel void fgp_clear( __write_only image2d_t out )\n" \
"{\n" \
"	int w = get_image_width( out );\n" \
"	int h = get_image_height( out );\n" \
"\n" \
"    int2 coord;\n" \
"	coord.x = get_global_id( 0 );\n" \
"	coord.y = get_global_id( 1 );\n" \
"\n" \
"	if( coord.x < w && coord.y < h ) {\n" \
"		write_imagef( out, coord, ( float4 ) 0.0f );\n" \
"	}\n" \
"}\n" \
"\n";
#endif
