#ifndef CVT_CLNDRANGE_H
#define CVT_CLNDRANGE_H

#include <iostream>

namespace cvt {
	/**
	  \ingroup CL
	 */
	class CLNDRange
	{
		friend std::ostream& operator<<( std::ostream& out, const CLNDRange& ndrange );
		public:
			CLNDRange();
			CLNDRange( size_t x );
			CLNDRange( size_t x, size_t y );
			CLNDRange( size_t x, size_t y, size_t z );
			CLNDRange( const CLNDRange& x );

			/**
			  Dimension of the CLNDRange object
			 */
			size_t dimension() const { return _dimension; }
			/**
			  Pointer to the range values
			  */
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

	/**
	  One dimensional CLNDRange
	  */
	inline CLNDRange::CLNDRange( size_t x  ) : _dimension( 1 )
	{
		_ndrange[ 0 ] = x;
		_ndrange[ 1 ] = 0;
		_ndrange[ 2 ] = 0;
	}

	/**
	  Two dimensional CLNDRange
	  */
	inline CLNDRange::CLNDRange( size_t x, size_t y ) : _dimension( 2 )
	{
		_ndrange[ 0 ] = x;
		_ndrange[ 1 ] = y;
		_ndrange[ 2 ] = 0;
	}

	/**
	  Three dimensional CLNDRange
	  */
	inline CLNDRange::CLNDRange( size_t x, size_t y, size_t z  ) : _dimension( 3 )
	{
		_ndrange[ 0 ] = x;
		_ndrange[ 1 ] = y;
		_ndrange[ 2 ] = z;
	}

	/**
	  Construct CLNDRange from other CLNDRange object
	*/
	inline CLNDRange::CLNDRange( const CLNDRange& x )
	{
		_dimension = x._dimension;
		_ndrange[ 0 ] = x._ndrange[ 0 ];
		_ndrange[ 1 ] = x._ndrange[ 1 ];
		_ndrange[ 2 ] = x._ndrange[ 2 ];
	}


	inline std::ostream& operator<<( std::ostream& out, const CLNDRange& r )
	{
		out << "CLNDRange: " << r._dimension << " ( " << r._ndrange[ 0 ] << " , " << r._ndrange[ 1 ] << " , " << r._ndrange[ 2 ] << " )\n";
		return out;
	}
}

#endif
