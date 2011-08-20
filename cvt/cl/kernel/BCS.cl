__kernel void BCS( __write_only image2d_t out,  __read_only image2d_t in, float4 rvec, float4 gvec, float4 bvec )
{
	const sampler_t sampler = CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_NONE | CLK_FILTER_NEAREST;
	float4 in, tmp, out;
	int2 coord;

	coord.x = get_global_id( 0 );
	coord.y = get_global_id( 1 );
	in = read_imagef( in, sampler, coord );
	tmp = in;
    tmp.w = 1.0f;	
	out.x = dot( rvec, tmp );
	out.y = dot( gvec, tmp );
	out.z = dot( bvec, tmp );
	out.w = in.w;
	write_imagef( out, coord, pixel );
}


