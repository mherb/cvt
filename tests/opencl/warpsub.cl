
__kernel void WARPSUB( __write_only image2d_t out,  __read_only image2d_t in,  __read_only image2d_t warp, __read_only image2d_t sub, const float mul )
{
	const sampler_t samplernn = CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_CLAMP_TO_EDGE | CLK_FILTER_NEAREST;
	const sampler_t sampler = CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_CLAMP_TO_EDGE | CLK_FILTER_LINEAR;
	int2 coord, coordw;
    float2 coordin;
	float4 in1, in2, v[ 2 ], o;
	float alpha, v1, v2, d;
    uint idx;
	float x;
/*	const float4 _e[ 4 ] = { 1.0f, 0.0f, 0.0f, 0.0f,
							 0.0f, 1.0f, 0.0f, 0.0f,
					         0.0f, 0.0f, 1.0f, 0.0f,
							 0.0f, 0.0f, 0.0f, 1.0f };*/


	coord.x = get_global_id( 0 );
    coord.y = get_global_id( 1 );

	coordw.x =  coord.x * 2;
	coordw.y =  coord.y;
    in1 = read_imagef( warp, samplernn, coordw );
    in2 = read_imagef( warp, samplernn, coordw + ( int2 ) ( 1, 0 ) );

#define index(v,i) (((float*)&(v))[(i)])//( dot( v, _e[ i ] ) )

	coordin = in1.xy;
	x = coordin.x + ( ( float ) coord.x ) * 4.0f + 0.0f;
	alpha = fract( x, &d );
	idx = ( ( int ) d ) & 0x03;
	coordin.x = ( float ) ( ( ( int ) d ) >> 2 ) + 0.5f;
	coordin.y = ( float ) coord.y + coordin.y + 0.5f;
	v[ 0 ] = read_imagef( in, sampler, coordin );
	v[ 1 ] = read_imagef( in, sampler, coordin + ( float2 ) ( 1.0f, 0.0f ) );
	v1 = index( v, idx );
	v2 = index( v, idx + 1 );
	o.x = mix( v1, v2, alpha );

	coordin = in1.zw;
	x = coordin.x + ( ( float ) coord.x ) * 4.0f + 1.0f;
	alpha = fract( x, &d );
	idx = ( ( int ) d ) & 0x03;
	coordin.x = ( float ) ( ( ( int ) d ) >> 2 ) + 0.5f;
	coordin.y = ( float ) coord.y + coordin.y + 0.5f;
	v[ 0 ] = read_imagef( in, sampler, coordin );
	v[ 1 ] = read_imagef( in, sampler, coordin + ( float2 ) ( 1.0f, 0.0f ) );
	v1 = index( v, idx );
	v2 = index( v, idx + 1 );
	o.y = mix( v1, v2, alpha );

	coordin = in2.xy;
	x = coordin.x + ( ( float ) coord.x ) * 4.0f + 2.0f;
	alpha = fract( x, &d );
	idx = ( ( int ) d ) & 0x03;
	coordin.x = ( float ) ( ( ( int ) d ) >> 2 ) + 0.5f;
	coordin.y = ( float ) coord.y + coordin.y + 0.5f;
	v[ 0 ] = read_imagef( in, sampler, coordin );
	v[ 1 ] = read_imagef( in, sampler, coordin + ( float2 ) ( 1.0f, 0.0f ) );
	v1 = index( v, idx );
	v2 = index( v, idx + 1 );
	o.z = mix( v1, v2, alpha );

	coordin = in2.zw;
	x = coordin.x + ( ( float ) coord.x ) * 4.0f + 3.0f;
	alpha = fract( x, &d );
	idx = ( ( int ) d ) & 0x03;
	coordin.x = ( ( float ) ( ( ( int ) d ) >> 2 ) ) + 0.5f;
	coordin.y = ( float ) coord.y + coordin.y + 0.5f;
	v[ 0 ] = read_imagef( in, sampler, coordin );
	v[ 1 ] = read_imagef( in, sampler, coordin + ( float2 ) ( 1.0f, 0.0f ) );
	v1 = index( v, idx );
	v2 = index( v, idx + 1 );
	o.w = mix( v1, v2, alpha );

	o = o - read_imagef( sub, samplernn, coord );
	o = o * mul;
    write_imagef( out, coord, o );
}
