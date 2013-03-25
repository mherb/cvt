__kernel void gradxy( __write_only image2d_t out, __read_only image2d_t src  )
{
	const sampler_t sampler = CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_CLAMP | CLK_FILTER_NEAREST;
	const int gx = get_global_id( 0 );
	const int gy = get_global_id( 1 );
	const int lx = get_local_id( 0 );
	const int ly = get_local_id( 1 );
	const int lw = get_local_size( 0 );
	const int lh = get_local_size( 1 );
	const int width = get_image_width( out );
	const int height = get_image_height( out );
	const int2 base = ( int2 )( get_group_id( 0 ) * lw - 1, get_group_id( 1 ) * lh - 1 );
	const int bstride = lw + 2;
	float dx, dy;
	local float buf[ 18 ][ 18 ];
	const float4 grayWeight =  ( float4 ) ( 0.2126f, 0.7152f, 0.0722f, 0.0f );
//	const float4 grayWeight =  ( float4 ) ( 0.333f, 0.333f, 0.333f, 0.0f );

//	float4 c = read_imagef( src, sampler, ( int2 ) ( gx, gy ) );

	for( int y = ly; y < lh + 2; y += lh ) {
		for( int x = lx; x < lw + 2; x += lw ) {
			buf[ y ][ x ] = dot( read_imagef( src, sampler, base + ( int2 )( x, y ) ), grayWeight );
		}
	}
	barrier( CLK_LOCAL_MEM_FENCE );

#define BUF( x, y ) buf[ y + 1 ][ x + 1 ]

	if( gx >= width || gy >= height )
		return;

	dx = ( BUF( lx + 1, ly ) - BUF( lx - 1 , ly ) );// * 0.5f + ( BUF( lx + 1, ly - 1 ) - BUF( lx - 1, ly - 1  ) ) * 0.25f + ( BUF( lx + 1, ly + 1 ) - BUF( lx - 1, ly + 1 ) ) * 0.25f;
	dy = ( BUF( lx, ly + 1 ) - BUF( lx, ly - 1 ) );// * 0.5f + ( BUF( lx - 1, ly + 1 ) - BUF( lx - 1, ly - 1 ) ) * 0.25f + ( BUF( lx + 1, ly + 1 ) - BUF( lx + 1, ly - 1 ) ) * 0.25f ;
//	float dxx = 0.125f * ( - BUF( lx + 1, ly ) * 0.5f - BUF( lx, ly + 1 ) * 0.5f + BUF( lx, ly ) );
//	float dyy = 0.125f * ( - BUF( lx - 1, ly ) * 0.5f - BUF( lx, ly - 1 ) * 0.5f +  BUF( lx, ly ) );
	float dxy = BUF( lx + 1, ly + 1 ) - BUF( lx - 1, ly - 1 );
	float dyx = BUF( lx - 1, ly + 1 ) - BUF( lx + 1, ly - 1 );

//	float lap = - BUF( lx, ly + 1 ) * 0.5f
//				- BUF( lx, ly - 1 ) * 0.5f
//				- BUF( lx + 1, ly ) * 0.25f
//				- BUF( lx - 1, ly ) * 0.25f
//				+  BUF( lx, ly );

	write_imagef( out,( int2 )( gx, gy ), ( float4 ) ( dx, dy, dxy, dyx ) );
}
