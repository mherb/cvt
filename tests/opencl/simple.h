#ifndef CL_simple_H
#define CL_simple_H
static const char _simple_source[ ] =
"__kernel void simple( __write_only image2d_t out, float4 color )\n" \
"{\n" \
"    int2 coord;\n" \
"    coord.x = get_global_id( 0 );\n" \
"    coord.y = get_global_id( 1 );\n" \
"    write_imagef( out, coord, color );\n" \
"}\n" \
"\n" \
"\n";
#endif
