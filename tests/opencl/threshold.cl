//void threshold( Image* idst, Image* isrc, Image* ig2, Image* it, Image* ix, Image* iy, Image* isrc0, float lambdatheta )
__kernel void THRESHOLD( __write_only image2d_t iout,
						  __read_only image2d_t src, __read_only image2d_t ig2, __read_only image2d_t it,
						 __read_only image2d_t ix, __read_only image2d_t iy, __read_only image2d_t src0 , const float lambdatheta )
{
	const sampler_t sampler = CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_CLAMP_TO_EDGE | CLK_FILTER_NEAREST;
	float4 dt, dx, dy, th, dxy, v, in1, in2, in10, in20, out;
	float4 ux, uy;
	int2 coord;
	int2 cmp;

    coord.x = get_global_id( 0 );
    coord.y = get_global_id( 1 );

	dt = read_imagef( it, sampler, coord );
	dx = read_imagef( ix, sampler, coord );
	dy = read_imagef( iy, sampler, coord );
	dxy = read_imagef( ig2, sampler, coord );
	th = dxy * lambdatheta;

	coord.x = coord.x * 2;
	in1 = read_imagef( src, sampler, coord );
	in2 = read_imagef( src, sampler, coord + ( int2 ) ( 1, 0 ) );
	in10 = read_imagef( src0, sampler, coord );
	in20 = read_imagef( src0, sampler, coord  + ( int2 ) ( 1, 0 ) );

	ux = ( float4 ) ( ( in1.xz - in10.xz ), ( in2.xz - in20.xz ) );
	uy = ( float4 ) ( ( in1.yw - in10.yw ), ( in2.yw - in20.yw ) );
	v = dt + ux * dx +  uy * dy;

	out.xz = in1.xz - v.xy * dx.xy / dxy.xy;
	out.yw = in1.yw - v.xy * dy.xy / dxy.xy;
	cmp = isless( v.xy, -th.xy );
	out = select( out, in1 + lambdatheta * ( float4 ) ( dx.x, dy.x, dx.y, dy.y ), ( int4 ) cmp.xxyy );
	cmp = isgreater( v.xy, th.xy );
	out = select( out, in1 - lambdatheta * ( float4 ) ( dx.x, dy.x, dx.y, dy.y ), ( int4 ) cmp.xxyy );
	write_imagef( iout, coord, out );

	out.xz = in2.xz - v.zw * dx.zw / dxy.zw;
	out.yw = in2.yw - v.zw * dy.zw / dxy.zw;
	cmp = isless( v.zw, -th.zw );
	out = select( out, in2 + lambdatheta * ( float4 ) ( dx.z, dy.z, dx.w, dy.w ), ( int4 ) cmp.xxyy );
	cmp = isgreater( v.zw, th.zw );
	out = select( out, in2 - lambdatheta * ( float4 ) ( dx.z, dy.z, dx.w, dy.w ), ( int4 ) cmp.xxyy );
	write_imagef( iout, coord + ( int2 ) ( 1, 0 ), out );
}
