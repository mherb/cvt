/* @param dmap		output of qudratic search
   @param input		current estimate of depth, plus C_{max} - C_{min}
   @param cv		cost volume
   @param depth		depth of the cost volume
   @param theta
   @param lambda
  */


__kernel void stereoCV_QSearch( __write_only image2d_t dmap, __read_only image2d_t input, global const float* cv, int depth, const float theta, const float lambda )
{
	const sampler_t sampler = CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_NONE | CLK_FILTER_NEAREST;
	const int gx = get_global_id( 0 );
	const int gy = get_global_id( 1 );
	const int width = get_image_width( dmap );
	const int height = get_image_height( dmap );
	const int stride = mul24( width, height );
	global float* cvptr = cv + ( mul24( gy, width ) + gx );
	int idx, didx, cmp;
	float val, nval;
	float4 in;

	if( gx >= width || gy >= height )
		return;

	/* ( depth-estimate, C_max - C_min, )*/
	in = read_imagef( input, sampler, ( int2 ) ( gx, gy ) );

#define EVALQ( dnew, C ) ( ( ( dnew - didx ) * ( dnew - didx ) ) / ( 2.0f * theta ) + lambda * C )

	// eval at
	didx = in.x * ( float ) depth;
	didx = clamp( didx, 0, depth - 1 );
	idx = ( int ) ( didx + 0.5f );
	val = EVALQ( ( float ) idx, cvptr[ idx * stride ] );
//	int r = ( int ) ( 2.0f * theta * lambda * in.y + 0.5f );
//	int dmin = clamp( idx - r, 0, depth );
//	int dmax = clamp( idx + r, 0, depth );
	int dmin = 0;
	int dmax = depth;

	for( int d = dmin; d < dmax; d++ ) {
		nval = EVALQ( ( ( float ) d / ( float ) depth ), cvptr[ d * stride ] );
		cmp = isless( nval, val );
		val = select( val, nval, cmp );
		idx = select( idx, d, cmp );
	}

	in.x = ( float ) idx / ( float ) depth;
	write_imagef( dmap, ( int2 ) ( gx, gy ), in );
}
