float bspline( float x )
{
	x = min( fabs( x ), 2.0f );
	if( x > 1.0f ) {
		x = ( 2.0f - x );
		return x * x * x / 6.0f;
	}
	return ( 4.0f - 6.0f * x * x + 3.0f * x * x *x ) / 6.0f;
}

void atom_addfl( __local float* dst, float val )
{
	atomic_xchg( dst, *dst + val );
}

void atom_addfg( __global float* dst, float val )
{
	atomic_xchg( dst, *dst + val );
}


__kernel void bhist( __global float* hist, int len, __read_only image2d_t img, __local float* lhist )
{
	const sampler_t sampler = CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_NONE | CLK_FILTER_NEAREST;
	int2 coord;
    coord.x = get_global_id( 0 );
    coord.y = get_global_id( 1 );

	if( get_local_id( 0 ) == 0 && get_local_id( 1 ) == 0 ) {
		for( int i = 0; i < len; i++ )
			lhist[ i ] = 0;
	}

	float v = read_imagef( img, sampler, coord ).x;
	float fidx = v * ( float ) ( len - 3 ) + 1.0f;
	float idx = floor( fidx );
	float4 values;
	values.x = bspline( - fidx + idx - 1 );
	values.y = bspline( -fidx + idx );
	values.z = bspline( - fidx + idx + 1 );
	values.w = bspline( - fidx + idx + 2 );

	barrier( CLK_LOCAL_MEM_FENCE );

	int i = ( int ) idx;
	atom_addfl( &lhist[ i - 1 ], values.x );
	atom_addfl( &lhist[ i + 0 ], values.y );
	atom_addfl( &lhist[ i + 2 ], values.z );
	atom_addfl( &lhist[ i + 3 ], values.w );

	barrier( CLK_LOCAL_MEM_FENCE );

	if( get_local_id( 0 ) == 0 && get_local_id( 1 ) == 0 ) {
		for( int i = 0; i < len; i++ )
			hist[ i ] += lhist[ i ];
	}
}
