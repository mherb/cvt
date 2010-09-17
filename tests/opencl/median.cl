__kernel void MEDIAN3( __write_only image2d_t out, __read_only image2d_t in, __local float4* buf )
{
	const sampler_t sampler = CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_CLAMP_TO_EDGE | CLK_FILTER_NEAREST;
	const int lx = get_local_id( 0 );
	const int ly = get_local_id( 1 );
	const int lw = get_local_size( 0 );
	const int lh = get_local_size( 1 );
	int2 coord;
	float2 v[ 9 ];
	float2 temp;
	float4 o;

	coord.x = get_global_id( 0 );
	coord.y = get_global_id( 1 );


#define BUF( x, y ) buf[ mul24( ( ( y ) + 1 ), ( lw + 2 ) ) + ( x ) + 1 ]
	BUF( lx, ly ) = read_imagef( in, sampler, coord );

	if( lx == lw - 1 ) {
		BUF( lx + 1, ly ) = read_imagef( in, sampler, coord + ( int2 )( 1, 0 ) );
		if( ly == lh - 1 )
			BUF( lx + 1, ly + 1 ) = read_imagef( in, sampler, coord + ( int2 )( 1, 1 ) );
		else if( ly == 0 )
			BUF( lx + 1, ly - 1 ) = read_imagef( in, sampler, coord + ( int2 )( 1, -1 ) );
	} else if( lx == 0 ) {
		BUF( lx - 1, ly ) = read_imagef( in, sampler, coord - ( int2 )( 1, 0 ) );
		if( ly == lh - 1 )
			BUF( lx - 1, ly + 1 ) = read_imagef( in, sampler, coord + ( int2 )( -1, 1 ) );
		else if( ly == 0 )
			BUF( lx - 1, ly - 1 ) = read_imagef( in, sampler, coord - ( int2 )( 1, 1 ) );
	}

	if( ly == lh - 1 ) {
		BUF( lx, ly + 1 ) = read_imagef( in, sampler, coord + ( int2 )( 0, 1 ) );
	} else if( ly == 0 ) {
		BUF( lx, ly - 1 ) = read_imagef( in, sampler, coord - ( int2 )( 0, 1 ) );
	}

    barrier( CLK_LOCAL_MEM_FENCE );

#define sort2(a, b)				temp = a; a = fmin(a, b); b = fmax(temp, b)
#define sort3( a, b, c )		sort2( a, b );sort2( b, c );sort2( a, b )

	v[ 0 ] = BUF( lx, ly ).xy;
	v[ 1 ] = BUF( lx, ly ).zw;
	v[ 2 ] = BUF( lx, ly - 1 ).xy;
	v[ 3 ] = BUF( lx, ly - 1 ).zw;
	v[ 4 ] = BUF( lx, ly + 1 ).xy;
	v[ 5 ] = BUF( lx, ly + 1 ).zw;
	v[ 6 ] = BUF( lx - 1, ly - 1 ).zw;
	v[ 7 ] = BUF( lx - 1, ly ).zw;
	v[ 8 ] = BUF( lx - 1, ly + 1 ).zw;

	sort3( v[ 0 ], v[ 1 ], v[ 2 ] );
	sort3( v[ 3 ], v[ 4 ], v[ 5 ] );
	sort3( v[ 6 ], v[ 7 ], v[ 8 ] );
	sort3( v[ 1 ], v[ 4 ], v[ 7 ] );
	sort3( v[ 6 ], v[ 4 ], v[ 2 ] );
	o.xy = v[ 4 ];

	v[ 0 ] = BUF( lx, ly ).xy;
	v[ 1 ] = BUF( lx, ly ).zw;
	v[ 2 ] = BUF( lx, ly - 1 ).xy;
	v[ 3 ] = BUF( lx, ly - 1 ).zw;
	v[ 4 ] = BUF( lx, ly + 1 ).xy;
	v[ 5 ] = BUF( lx, ly + 1 ).zw;
	v[ 6 ] = BUF( lx + 1, ly - 1 ).xy;
	v[ 7 ] = BUF( lx + 1, ly ).xy;
	v[ 8 ] = BUF( lx + 1, ly + 1 ).xy;

	sort3( v[ 0 ], v[ 1 ], v[ 2 ] );
	sort3( v[ 3 ], v[ 4 ], v[ 5 ] );
	sort3( v[ 6 ], v[ 7 ], v[ 8 ] );
	sort3( v[ 1 ], v[ 4 ], v[ 7 ] );
	sort3( v[ 6 ], v[ 4 ], v[ 2 ] );
	o.zw = v[ 4 ];

    write_imagef( out, coord, o );
}
