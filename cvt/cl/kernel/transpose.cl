/*
	This kernel needs the defines TYPE and BLOCK_DIM
	the local memory 'block' must have the size ( BLOCK_DIM + 1) * BLOCK_DIM * sizeof( TYPE )
 */

__kernel void transpose( __global TYPE* odata, __global TYPE* idata, int width, int height, __local TYPE* block )
{
	// read the matrix tile into shared memory
	unsigned int xIndex = get_global_id( 0 );
	unsigned int yIndex = get_global_id( 1 );

	if( ( xIndex < width ) && ( yIndex < height ) ) {
		unsigned int index_in = yIndex * width + xIndex;
		block[ get_local_id( 1 ) * ( BLOCK_DIM + 1 ) + get_local_id( 0 ) ] = idata[ index_in ];
	}

	barrier( CLK_LOCAL_MEM_FENCE );

	// write the transposed matrix tile to global memory
	xIndex = get_group_id( 1 ) * BLOCK_DIM + get_local_id( 0 );
	yIndex = get_group_id( 0 ) * BLOCK_DIM + get_local_id( 1 );
	if( ( xIndex < height ) && ( yIndex < width ) ) {
		unsigned int index_out = yIndex * height + xIndex;
		odata[ index_out ] = block[ get_local_id( 0 ) * ( BLOCK_DIM + 1 ) + get_local_id( 1 ) ];
	}
}

