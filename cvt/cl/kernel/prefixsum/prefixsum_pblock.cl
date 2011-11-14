__kernel void prefixsum_pblock( __write_only image2d_t out,  __read_only image2d_t in, __local float4* buf )
{
    const sampler_t sampler = CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_CLAMP | CLK_FILTER_NEAREST;
	const int lx = get_local_id( 0 );
	const int ly = get_local_id( 1 );
    const int lw = get_local_size( 0 );
    const int lh = get_local_size( 1 );
	const int width = get_image_width( out );
	const int height = get_image_height( out );
	int2 coord;
	const int oddx = lx & 1;
	const int oddy = ly & 1;
	int lid = mul24( lw, ly ) + lx;

	coord.x = get_global_id( 0 );
	coord.y = get_global_id( 1 );

	buf[ lid ] = read_imagef( in, sampler, coord );

	barrier( CLK_LOCAL_MEM_FENCE );

	// assume lw == lh
	for( int offset = 1; offset < lw; offset <<= 1 )
	{
		if( lx >= offset && oddx )
			buf[ lid ] += buf[ lid - offset ];
		barrier( CLK_LOCAL_MEM_FENCE );
		if( ly >= offset && oddy )
			buf[ lid ] += buf[ lid - mul24( lw, offset ) ];
		barrier( CLK_LOCAL_MEM_FENCE );
	}

	float4 tmp = buf[ lid ];

	if( lx > 1 && !oddx )
		tmp += buf[ lid - 1 ];
	if( ly > 1 && !oddy )
		tmp += buf[ lid - lw ];
	if( lx > 1 && ly > 1 && !oddx && !oddy )
		tmp += buf[ lid - lw - 1 ];

	if( coord.x < width && coord.y < height )
		write_imagef( out, coord, tmp );
}

