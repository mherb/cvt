//#include <cvt/math/VectorN.h>
#include <cvt/math/Matrix.h>

using namespace cvt;

template<typename T>
class LUDecomposition {
	public:
		LUDecomposition( const Matrix3<T>& m ) : _lu( m )
		{
			for( int k = 0; k < 3; k++ ) {
				for( int i = k + 1; i < 3; i++ ) {
					_lu[ i ][ k ] /= _lu[ k ][ k ];
					for( int l = k + 1; l < 3; l++ ) {
						_lu[ i ][ l ] -= _lu[ i ][ k ] * _lu[ k ][ l ];
					}
				}
			}
		}

		void solve( Vector3<T>& x, const Vector3<T>& b ) const
		{
			Vector3<T> tmp;
			fwdSubst( tmp, b );
			bwdSubst( x, tmp );
		}

	private:
		void fwdSubst( Vector3<T>& x, const Vector3<T>& b ) const
		{
			x[ 0 ] = b[ 0 ];
			for( int i = 1; i < 3; i++ ) {
				T sum = b[ i ];
				for( int k = 0; k < i; k++ )
					sum -= _lu[ i ][ k ] * x[ k ];
				x[ i ] = sum;
			}
		}

		void bwdSubst( Vector3<T>& x, const Vector3<T>& b ) const
		{
			x[ 2 ] = b[ 2 ] / _lu[ 2 ][ 2 ];
			for( int i = 1; i >= 0; i-- ) {
				T sum = 0;
				for( int k = i + 1; k < 3; k++ )
					sum += _lu[ i ][ k ] * x[ k ];
				x[ i ] = ( b[ i ] - sum ) / _lu[ i ][ i ];
			}
		}

		Matrix3<T> _lu;
};

template<typename T>
class LUPDecomposition {
	public:
		LUPDecomposition( const Matrix3<T>& m ) : _lu( m )
		{
			for( int k = 0; k < 3; k++ ) {
				// find row with maximum value at k
				_p[ k ] = k;
				for( int i = k + 1; i < 3; i++ ) {
					if( _lu[ _p[ k ] ][ k ] < _lu[ i ][ k ] )
						_p[ k ] = i;
				}
				// swap if necessary
				if( _p[ k ] != k ) {
					T tmp;
					for( int i = 0; i < 3; i++ ) {
						tmp = _lu[ k ][ i ];
						_lu[ k ][ i ] = _lu[ _p[ k ] ][ i ];
						_lu[ _p[ k ] ][ i ] = tmp;
					}
				}
				// normal LU
				for( int i = k + 1; i < 3; i++ ) {
					_lu[ i ][ k ] /= _lu[ k ][ k ];
					for( int l = k + 1; l < 3; l++ ) {
						_lu[ i ][ l ] -= _lu[ i ][ k ] * _lu[ k ][ l ];
					}
				}
			}
		}

		void solve( Vector3<T>& x, const Vector3<T>& b ) const
		{
			Vector3<T> tmp;
			fwdSubst( tmp, b );
			bwdSubst( x, tmp );
		}

	private:
		void fwdSubst( Vector3<T>& x, const Vector3<T>& b ) const
		{
			x = b;
			x[ _p[ 0 ] ] = b[ 0 ];
			x[ 0 ] = b[ _p[ 0 ] ];
			for( int i = 1; i < 3; i++ ) {
				T sum = x[ _p[ i ] ];
				x[ _p[ i ] ] = x[ i ];
				for( int k = 0; k < i; k++ )
					sum -= _lu[ i ][ k ] * x[ k ];
				x[ i ] = sum;
			}
		}

		void bwdSubst( Vector3<T>& x, const Vector3<T>& b ) const
		{
			x[ 2 ] = b[ 2 ] / _lu[ 2 ][ 2 ];
			for( int i = 1; i >= 0; i-- ) {
				T sum = 0;
				for( int k = i + 1; k < 3; k++ )
					sum += _lu[ i ][ k ] * x[ k ];
				x[ i ] = ( b[ i ] - sum ) / _lu[ i ][ i ];
			}
		}

		Matrix3<T> _lu;
		int		   _p[ 3 ];
};



int main()
{
/*	Vector<float> a( 10 );
	Vector<float> b( 10 );
	Vector<float> c( 10 );

	a.fill( 0 );
	b.fill( 1 );
	c.fill( 2 );

	a = 5.0f *  b * 2.0f + 2.0f * c;


	for( size_t i = 0; i < a.size(); i++ )
		std::cout << a[ i ] << std::endl;*/

/*	Matrix3f A( 1.0f, 4.0f, 3.0f,
			    2.0f, 2.0f, 8.0f,
				0.01f, 6.0f, 100.0f );*/

	Matrix3f A( 3.0f, 17.0f, 10.0f,
			    4.0f,  4.0f, -2.0f,
				6.0f, 18.0f, -12.0f );

	{
		Matrix3f mat( A );

		mat[ 1 ].x /= mat[ 0 ].x;
	mat[ 2 ].x /= mat[ 0 ].x;

	mat[ 1 ].y -= mat[ 1 ].x * mat[ 0 ].y;
	mat[ 2 ].y -= mat[ 2 ].x * mat[ 0 ].y;
	mat[ 1 ].z -= mat[ 1 ].x * mat[ 0 ].z;
	mat[ 2 ].z -= mat[ 2 ].x * mat[ 0 ].z;

	mat[ 2 ].y /= mat[ 1 ].y;
	mat[ 2 ].z -= mat[ 2 ].y * mat[ 1 ].z;


//		std::cout << mat << std::endl;
	}
/*	std::cout << A << std::endl;
	for( int k = 0; k < 3; k++ ) {
		for( int i = k + 1; i < 3; i++ ) {
			A[ i ][ k ] /= A[ k ][ k ];
			for( int l = k + 1; l < 3; l++ ) {
				A[ i ][ l ] -= A[ i ][ k ] * A[ k ][ l ];
			}
		}
	}

	std::cout << A << std::endl;*/
	LUPDecomposition<float> lup( A );
	LUDecomposition<float> lu( A );
	Vector3f b( 5.0f, 1.0f, 3.0f );
	Vector3f x;
	lup.solve( x, b );
	std::cout << x << std::endl;
	lu.solve( x, b );
	std::cout << x << std::endl;
	std::cout << A.inverse() * b << std::endl;
}
