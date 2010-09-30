__kernel void gaussiir( __write_only image2d_t output, __read_only image2d_t input, __global float4* buffer, int w, float a0, float a1, float a2, float a3, float b1, float b2, float coefp, float coefn )
{
	const sampler_t sampler = CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_CLAMP_TO_EDGE | CLK_FILTER_NEAREST;
	int2 coord;
	coord.x = 0;
    coord.y = get_global_id( 0 );
    float4 xp;
    float4 yp;
    float4 yb;
    float4 xn;
    float4 xa;
    float4 yn;
    float4 ya;

	  // forward pass
	xp = read_imagef( input, sampler, coord );
	yb = coefp * xp;
	yp = yb;
    for (int x = 0; x < w; x++) {
		coord.x = x;
        float4 xc = read_imagef( input, sampler, coord );
        float4 yc = a0 * xc + a1 * xp - b1 * yp - b2 * yb;
		buffer[ x ] = yc;
        xp = xc;
		yb = yp;
		yp = yc;
    }

	coord.x = w - 1;
    // reverse pass
    xn = xa = read_imagef( input, sampler, coord );
	yn = coefn*xn;
	ya = yn;
    for (int x = w-1; x >= 0; x--) {
		coord.x = x;
        float4 xc = read_imagef( input, sampler, coord );
        float4 yc = a2 * xn + a3 * xa - b1 * yn - b2 * ya;
        xa = xn;
		xn = xc;
		ya = yn;
		yn = yc;
		write_imagef( output, coord, buffer[ x ] + yc );
    }
}
