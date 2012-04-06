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

#define EVALQ( dnew, C ) ( ( ( dnew - in.x ) * ( dnew - in.x ) ) / ( 2.0f * theta ) + lambda * C )

	val = 1e10f;
	idx = depth;
	// eval at
//	didx = in.x * ( float ) depth;
//	didx = clamp( didx, 0, depth - 1 );
//	idx = ( int ) ( didx + 0.5f );
//	val = EVALQ( ( ( float ) idx / ( float ) depth ), cvptr[ idx * stride ] );
//	int r = ( int ) ( 2.0f * theta * lambda * in.y + 0.5f );
//	int dmin = clamp( idx - r, 0, depth );
//	int dmax = clamp( idx + r, 0, depth );
	int dmin = 0;
	int dmax = depth;
	float scale = depth - 1;

	for( int d = dmin; d < dmax; d++ ) {
		nval = EVALQ( ( ( float ) d / scale ), cvptr[ d * stride ] );
//		if( nval < val ) {
//			val = nval;
//			idx = d;
//		}
		cmp = isless( nval, val );
		val = select( val, nval, cmp );
		idx = select( idx, d, cmp );
	}

#if 1
	if( idx >= 2 && idx < depth - 2 ) {
		float dx = EVALQ( ( ( float ) ( idx + 1 ) / scale ), cvptr[ ( idx + 1 ) * stride ] )
					- EVALQ( ( ( float ) ( idx - 1 ) / scale ), cvptr[ ( idx - 1 ) * stride ] );
		float d2x = EVALQ( ( ( float ) ( idx + 1 ) / scale ), cvptr[ ( idx + 1 ) * stride ] )
					+ EVALQ( ( ( float ) ( idx - 1 ) / scale ), cvptr[ ( idx - 1 ) * stride ] )
					- 2.0f * EVALQ( ( ( float ) ( idx ) / scale ), cvptr[ ( idx ) * stride ] );
//		float d2x = - dx + ( EVALQ( ( ( float ) ( idx ) / scale ), cvptr[ ( idx ) * stride ] )
//					- EVALQ( ( ( float ) ( idx + 2 ) / scale ), cvptr[ ( idx + 2 ) * stride ] ) );




		in.x = ( ( float ) idx - 1.0f *  dx / d2x ) / scale;
	} else
#endif
		in.x = ( float ) idx / scale;
	write_imagef( dmap, ( int2 ) ( gx, gy ), in );
}
