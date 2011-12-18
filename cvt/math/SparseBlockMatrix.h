#ifndef CVT_SPARSE_BLOCK_MATRIX_H
#define CVT_SPARSE_BLOCK_MATRIX_H

namespace cvt
{
	template<size_t bRows, size_t bCols>
	class SparseBlockMatrix
	{
		public:
			typedef Eigen::Matrix<double, bRows, bCols> BlockMatType;
			
			SparseBlockMatrix();
			~SparseBlockMatrix();


			void resize( size_t numRowBlocks, size_t numColBlocks );

			bool containsBlock( size_t row, size_t col ) const;

			BlockMatType&	block( size_t row, size_t col );

		private:
			int*	_filled;
			size_t	_numRows;
			size_t	_numCols;
			std::vector<BlockMatType, Eigen::aligned_allocator>	_blocks;
	};

	template <size_t bRows, size_t bCols>
	inline SparseBlockMatrix<bRows, bCols>::SparseBlockMatrix() :
		_filled( 0 ),
		_numRows( 0 ),
		_numCols( 0 )
	{
	}
	
	template <size_t bRows, size_t bCols>
	inline SparseBlockMatrix<bRows, bCols>::~SparseBlockMatrix() 
	{
		if( _filled ){
			delete[] _filled;
		}
	}

	template <size_t bRows, size_t bCols>
	inline bool SparseBlockMatrix<bRows, bCols>::containsBlock() const
	{
		return _filled[ row * _numCols + row  ] != -1;
	}

	template <size_t bRows, size_t bCols>
	inline BlockMatType& SparseBlockMatrix<bRows, bCols>::block( size_t r, size_t c )
	{
		int* a = _filled[ row * _numCols + row  ];

		if( *a == -1 ){
			*a = _blocks.size();
			_blocks.push_back( BlockMatType::Zero() );
		}

		return _blocks[ *a ];
	}
	
	template <size_t bRows, size_t bCols>
	inline void SparseBlockMatrix<bRows, bCols>::resize( size_t rows, size_t cols ) 	
	{
		if( _numRows == rows && _numCols == cols ) {
			_blocks.clear();
			size_t n = _numRows * _numCols;
			memset( filled, -1, sizeof( int ) * n );
			return;
		} else {
			delete[] _filled;
			_numRows = rows; 
			_numCols = cols;
			size_t n = _numRows * _numCols;
			filled = new int[ n ];
			memset( filled, -1, sizeof( int ) * n );
		}
	}
}

#endif
