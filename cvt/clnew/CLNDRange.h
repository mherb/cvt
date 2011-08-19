#ifndef CVT_CLNDRANGE_H
#define CVT_CLNDRANGE_H

namespace cvt {
	class CLNDRange
	{
		public:
			CLNDRange();
			CLNDRange( size_t x );
			CLNDRange( size_t x, size_t y );
			CLNDRange( size_t x, size_t y, size_t z );
			CLNDRange( const CLNDRange& x );

			size_t dimension() const { return _dimension; }
			const size_t* range() const { return _ndrange; }

		private:
			cl_uint _dimension;
			size_t _ndrange[ 3 ];
	};

	inline CLNDRange::CLNDRange() : _dimension( 0 )
	{
		_ndrange[ 0 ] = 0;
		_ndrange[ 1 ] = 0;
		_ndrange[ 2 ] = 0;
	}

	inline CLNDRange::CLNDRange( size_t x  ) : _dimension( 1 )
	{
		_ndrange[ 0 ] = x;
		_ndrange[ 1 ] = 0;
		_ndrange[ 2 ] = 0;
	}

	inline CLNDRange::CLNDRange( size_t x, size_t y ) : _dimension( 2 )
	{
		_ndrange[ 0 ] = x;
		_ndrange[ 1 ] = y;
		_ndrange[ 2 ] = 0;
	}

	inline CLNDRange::CLNDRange( size_t x, size_t y, size_t z  ) : _dimension( 3 )
	{
		_ndrange[ 0 ] = x;
		_ndrange[ 1 ] = y;
		_ndrange[ 2 ] = z;
	}

	inline CLNDRange::CLNDRange( const CLNDRange& x )
	{
		_dimension = x._dimension;
		_ndrange[ 0 ] = x._ndrange[ 0 ];
		_ndrange[ 1 ] = x._ndrange[ 1 ];
		_ndrange[ 2 ] = x._ndrange[ 2 ];
	}
}

#endif
