__kernel void Denoise_CALCP2( __write_only image2d_t dst,
							  __read_only image2d_t src,
							  __read_only image2d_t pxin, __read_only image2d_t pyin, const float lambda, __local float4* pxbuf, __local float4* pybuf )
{
	const sampler_t sampler = CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_CLAMP_TO_EDGE | CLK_FILTER_NEAREST;
	const int width = get_global_size( 0 );
	const int height = get_global_size( 1 );
	float4 i, px, py, pdx, pdy;
	int2 coord;
    const int lx = get_local_id( 0 );
    const int ly = get_local_id( 1 );
    const int lw = get_local_size( 0 );
    const int lh = get_local_size( 1 );

	coord.x = get_global_id( 0 );
	coord.y = get_global_id( 1 );

	px = read_imagef( pxin, sampler, coord );
	py = read_imagef( pyin, sampler, coord );

#define PXBUF( x, y ) pxbuf[ mul24(( y + 1 ), ( lw + 1 )) + ( x + 1 ) ]
#define PYBUF( x, y ) pybuf[ mul24(( y + 1 ), ( lw + 1 )) + ( x + 1 ) ]

    PXBUF( lx, ly ) = px;
    PYBUF( lx, ly ) = py;

	if( lx == 0 )
		PXBUF( lx - 1, ly ) = read_imagef( pxin, sampler, coord - ( int2 )( 1, 0 ) );
	if( ly == 0 )
		PYBUF( lx, ly - 1 ) = read_imagef( pyin, sampler, coord - ( int2 )( 0, 1 ) );


	barrier( CLK_LOCAL_MEM_FENCE );

	if( coord.x == width - 1 )
		pdx.zw = -px.xy;
	else
		pdx.zw = px.zw - px.xy;

	if( coord.x == 0)
		pdx.xy = px.xy;
	else
		pdx.xy = px.xy - PXBUF( lx - 1, ly ).zw;

	if( coord.y == 0 )
		pdy = py;
	else if( coord.y == height - 1 )
		pdy = - PYBUF( lx, ly - 1 );
	else
		pdy = py - PYBUF( lx, ly - 1 );

	i = read_imagef( src, sampler, coord );
	i = i - lambda * ( pdx + pdy );
    write_imagef( dst, coord, i );
}


