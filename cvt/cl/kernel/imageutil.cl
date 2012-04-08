__kernel void image_rgba_to_gray_x( __write_only image2d_t out, __read_only image2d_t in, int chan )
{
	int w = get_image_width( out );
	int h = get_image_height( out );
	const sampler_t sampler = CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_NONE | CLK_FILTER_NEAREST;

    int2 coord;
	coord.x = get_global_id( 0 );
	coord.y = get_global_id( 1 );

	if( coord.x >= w || coord.y >= h )
		return;

	float4 pixel = read_imagef( in, sampler, coord );
	write_imagef( out, coord, ( float4 ) 1.0f - *( ( float* )( &pixel ) + chan )
 );
}

__kernel __attribute__((reqd_work_group_size( 16, 16, 1))) void image_gradexp_to_x( __write_only image2d_t out, __read_only image2d_t in, __read_only image2d_t img, int chan )
{
	const int w = get_image_width( in );
	const int h = get_image_height( in );
	const int lx = get_local_id( 0 );
	const int ly = get_local_id( 1 );
	const sampler_t sampler = CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_NONE | CLK_FILTER_NEAREST;
	const sampler_t samplerclamp = CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_CLAMP_TO_EDGE | CLK_FILTER_NEAREST;
	const float4 dotmulGRAY = ( float4 ) (0.2126f, 0.7152f, 0.0722f, 0.0f );
	const int2 base = ( int2 )( ( get_group_id( 0 ) << 4 ) - 1,  ( get_group_id( 1 ) << 4 ) - 1 );
	local float buf[ 18 * 18 ];

    int2 coord;
	coord.x = get_global_id( 0 );
	coord.y = get_global_id( 1 );

	for( int y = ly; y < 18; y += 16 ) {
		for( int x = lx; x < 18; x += 16 ) {
			buf[ mul24( y, 18 ) + x ] = dot( read_imagef( img, samplerclamp, base + ( int2 )( x, y ) ), dotmulGRAY );
		}
	}

	barrier( CLK_LOCAL_MEM_FENCE );

	if( coord.x >= w || coord.y >= h )
		return;

#define BUF( x, y ) buf[ mul24( ( ( y ) + 1 ), 18 ) + ( x ) + 1 ]
#define ALPHA 4.0f
#define BETA 2.0f
#define GAMMA 1.0f
	float2 delta = ( float2 ) ( BUF( lx - 1, ly ) - BUF( lx , ly ), BUF( lx, ly - 1 ) - BUF( lx, ly ) );
	float weight = fmax( 1e-4f, GAMMA * exp( - BETA * pow( fast_length( delta ), ALPHA ) ) );
	float4 pixel = read_imagef( in, sampler, coord );
	*( ( float* )( &pixel ) + chan ) = weight;
	write_imagef( out, coord, ( float4 ) pixel );
}
