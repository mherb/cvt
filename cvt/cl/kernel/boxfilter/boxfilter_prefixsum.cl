/*void async_work_group_image_copy( __local float4* buf, __read_only image2d_t input, const sampler_t sampler, int4 xywh )
{
	const int lx = get_local_id( 0 );
	const int ly = get_local_id( 1 );
	const int lw = get_local_size( 0 );
	const int lh = get_local_size( 1 );
	const int width = get_image_width( input );
	const int height = get_image_height( input );

	int offsety = xywh.z * ly; // y-offset to buf for this work-item
	int incy = xywh.z * lh; // x-offset to buf for this work item

	for( int y = ly; y < xywh.w; y += lh ) {
		for( int x = lx, offsetx = lx; x < xywh.z; x += lw, offsetx += lw ) {
			buf[ offsety + offsetx ] = read_imagef( input, sampler, xywh.xy + ( int2 ) ( x, y ) );
		}
		offsety += incy;
	}
}*/


__kernel void boxfilter_prefixsum( __write_only image2d_t out,  __read_only image2d_t in, const int r )
{
	const sampler_t sampler = CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_NONE | CLK_FILTER_NEAREST;
	const int width = get_image_width( out );
	const int height = get_image_height( out );
	int2 coord, mincoord, maxcoord;
	float size;
	float4 value;

	coord.x = get_global_id( 0 );
	coord.y = get_global_id( 1 );

	if( coord.x >= width || coord.y >= height )
		return;

	mincoord.x = max( 0, coord.x - r -1 );
	mincoord.y = max( 0, coord.y - r -1 );
	maxcoord.x = min( width - 1, coord.x + r );
	maxcoord.y = min( height - 1, coord.y + r );

	size = ( maxcoord.x - mincoord.x ) * ( maxcoord.y - mincoord.y );

	value  = read_imagef( in, sampler, mincoord );
	value -= read_imagef( in, sampler, ( int2 )( mincoord.x, maxcoord.y ) );
	value -= read_imagef( in, sampler, ( int2 )( maxcoord.x, mincoord.y ) );
	value += read_imagef( in, sampler, maxcoord );

	value /= ( float4 ) size;

	write_imagef( out, coord, value );
}

