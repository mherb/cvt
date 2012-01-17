
__kernel void stereogcv_costmin( __write_only image2d_t costout, __read_only image2d_t newcost, __read_only image2d_t costin, float newdepth )
{
	const sampler_t sampler = CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_NONE | CLK_FILTER_LINEAR;
	int2 coord;
	const int width = get_image_width( costout );
	const int height = get_image_height( costout );
	float4 C, Cnew;

	coord.x = get_global_id( 0 );
	coord.y = get_global_id( 1 );

	if( coord.x >= width || coord.y >= height )
		return;

	C = read_imagef( costin, sampler, coord );
	Cnew.x = read_imagef( newcost, sampler, coord ).x;
//	Cnew.y = newdepth;

	if( Cnew.x <  C.x ) {
		C.x = Cnew.x;
		C.y = newdepth;
	}

	write_imagef( costout, coord, C );
//	int4 cmp = ( int4 )( Cold.x > Cnew.x );
//	write_imagef( costout, coord, select( Cold, Cnew, cmp ) );
}
