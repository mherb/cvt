#include <cvt/math/Matrix.h>
#include <cvt/math/Math.h>

using namespace cvt;

void jacobi_cs( float x, float y, float z )
{
	float tau = ( z - x ) / ( 2.0f * y );
	float t;
	if( tau >= 0 ) {
		t = 1.0f / ( tau + Math::sqrt( 1 + Math::sqr( tau ) ) );
	} else {
		t = -1.0f / ( -tau + Math::sqrt( 1 + Math::sqr( tau ) ) );
	}
	std::cout << "c: " << ( 1.0f / Math::sqrt( 1.0f  + t * t ) )
		      << " s: " << ( t / Math::sqrt( 1.0f  + t * t ) ) << std::endl;
}

#define ROTAPPLYLEFT3( m, c, s, i, k ) \
	do { \
		float tmp1, tmp2; \
		tmp1 = s * m[ k ][ i ] + m[ i ][ i ] * c; \
		tmp2 = c * m[ k ][ i ] - m[ i ][ i ] * s; \
		m[ i ][ i ] = tmp1; \
		m[ k ][ i ] = tmp2; \
		tmp1 = s * m[ k ][ k ] + m[ i ][ k ] * c; \
		tmp2 = c * m[ k ][ k ] - m[ i ][ k ] * s; \
		m[ i ][ k ] = tmp1; \
		m[ k ][ k ] = tmp2; \
		int h = 3 - i - k; \
		tmp1 = s * m[ k ][ h ] + m[ i ][ h ] * c; \
		tmp2 = c * m[ k ][ h ] - m[ i ][ h ] * s; \
		m[ i ][ h ] = tmp1; \
		m[ k ][ h ] = tmp2; \
	} while( 0 )

#define ROTAPPLYRIGHT3( m, c, s, i, k ) \
	do { \
		float tmp1, tmp2; \
		tmp1 = c * m[ i ][ i ] - m[ i ][ k ] * s; \
		tmp2 = s * m[ i ][ i ] + m[ i ][ k ] * c; \
		m[ i ][ i ] = tmp1; \
		m[ i ][ k ] = tmp2; \
		tmp1 = c * m[ k ][ i ] - m[ k ][ k ] * s; \
		tmp2 = s * m[ k ][ i ] + m[ k ][ k ] * c; \
		m[ k ][ i ] = tmp1; \
		m[ k ][ k ] = tmp2; \
		int h = 3 - i - k; \
		tmp1 = c * m[ h ][ i ] - m[ h ][ k ] * s; \
		tmp2 = s * m[ h ][ i ] + m[ h ][ k ] * c; \
		m[ h ][ i ] = tmp1; \
		m[ h ][ k ] = tmp2; \
	} while( 0 )

#define JACOBIAPPLY3( m, c, s, i, k ) \
	do { \
		float tmp1, tmp2; \
		tmp1 = c * c * m[ i ][ i ] + s * s * m[ k ][ k ] - 2.0f * s * c * m[ i ][ k ]; \
		tmp2 = s * s * m[ i ][ i ] + c * c * m[ k ][ k ] + 2.0f * s * c * m[ i ][ k ]; \
		m[ i ][ i ] = tmp1; \
		m[ k ][ k ] = tmp2; \
		m[ i ][ k ] = 0; \
		m[ k ][ i ] = 0; \
		int h = 3 - i - k; \
		tmp1 = c * m[ i ][ h ] - s * m[ k ][ h ]; \
		tmp2 = s * m[ i ][ h ] + c * m[ k ][ h ]; \
		m[ i ][ h ] = tmp1; \
		m[ k ][ h ] = tmp2; \
		tmp1 = c * m[ h ][ i ] - s * m[ h ][ k ]; \
		tmp2 = s * m[ h ][ i ] + c * m[ h ][ k ]; \
		m[ h ][ i ] = tmp1; \
		m[ h ][ k ] = tmp2; \
	} while( 0 )

#if 0
void svd2( Matrix2f& mat )
{
	float c, s;
	Matrix2f u, v;
	int i = 0, k = 1;

	v.identity();
	u.identity();
	std::cout << mat << std::endl;

	/* make 2 x 2 symmetric */
	Math::givens( c, s, mat[ i ][ i ] + mat[ k ][ k ], mat[ k ][ i ] - mat[ i ][ k ] );
	ROTAPPLYRIGHT3( mat, c, s, i, k );

	/* apply the givens rotation to v */
	ROTAPPLYLEFT3( v, c, -s, i, k );

	/* make 2 x 2 diagonal */
	Math::jacobi( c, s, mat[ i ][ i ], mat[ k ][ i ], mat[ k ][ k ] );
	JACOBIAPPLY3( mat, c, s, i, k );

	std::cout << mat << std::endl << std::endl;
	/* apply the jacobi rotation to V */
	ROTAPPLYLEFT3( v, c, -s, i, k );
	/* apply the jacobi rotation to U */
	ROTAPPLYRIGHT3( u, c, s, i, k );

	std::cout << ( u * mat * v ) << std::endl << std::endl;
}
#endif

void svd( Matrix3f& mat, Matrix3f& u, Matrix3f& v )
{
	float c, s;
	bool finished;

	u.setIdentity();
	v.setIdentity();

	/* diagonalize */
	do {
		finished = true;
		for( int i = 0; i < 2; i++ ) {
			for( int k = i + 1; k < 3; k++ ) {
				if( Math::abs( mat[ i ][ k ] ) >= Math::EPSILONF || Math::abs( mat[ k ][ i ] ) >= Math::EPSILONF  ) {
					finished = false;
					/* make 2 x 2 symmetric */
					Math::givens( c, s, mat[ i ][ i ] + mat[ k ][ k ], mat[ k ][ i ] - mat[ i ][ k ] );
					ROTAPPLYRIGHT3( mat, c, s, i, k );

					/* apply the inverse givens rotation to v */
					ROTAPPLYLEFT3( v, c, -s, i, k );

					/* make 2 x 2 diagonal, apply jacobi */
					Math::jacobi( c, s, mat[ i ][ i ], mat[ k ][ i ], mat[ k ][ k ] );
					JACOBIAPPLY3( mat, c, s, i, k );

					/* apply the inverse jacobi rotation to V */
					ROTAPPLYLEFT3( v, c, -s, i, k );

					/* apply the jacobi rotation to U */
					ROTAPPLYRIGHT3( u, c, s, i, k );
				}
			}
		}
	} while( !finished );

	/* make singular values positive */
	for( int i = 0; i < 3; i++ ) {
		if( mat[ i ][ i ] < 0 ) {
			mat[ i ][ i ] = - mat[ i ][ i ];
			for( int k = 0; k < 3; k++ )
				u[ k ][ i ] = -u[ k ][ i ];
		}
	}

	/* sort singular values */
	int imax;
	imax = ( mat[ 0 ][ 0 ] > mat[ 1 ][ 1 ] ) ? 0 : 1;
	if( mat[ imax ][ imax ] < mat[ 2 ][ 2 ] ) imax = 2;
	/* bring largest singular value to position 0 */
	if( imax != 0 ) {
		float tmp;
		for( int i = 0; i < 3; i++ ) {
			tmp = u[ i ][ 0 ];
			u[ i ][ 0 ] = u[ i ][ imax ];
			u[ i ][ imax ] = tmp;

			tmp = v[ 0 ][ i ];
			v[ 0 ][ i ] = v[ imax ][ i ];
			v[ imax ][ i ] = tmp;
		}
		tmp = mat[ 0 ][ 0 ];
		mat[ 0 ][ 0 ] = mat[ imax ][ imax ];
		mat[ imax ][ imax ] = tmp;
	}
	/* swap singular values 1 and 2 if necessary */
	if( mat[ 1 ][ 1 ] < mat[ 2 ][ 2 ]) {
		float tmp;
		for( int i = 0; i < 3; i++ ) {
			tmp = u[ i ][ 1 ];
			u[ i ][ 1 ] = u[ i ][ 2 ];
			u[ i ][ 2 ] = tmp;

			tmp = v[ 1 ][ i ];
			v[ 1 ][ i ] = v[ 2 ][ i ];
			v[ 2 ][ i ] = tmp;
		}
		tmp = mat[ 1 ][ 1 ];
		mat[ 1 ][ 1 ] = mat[ 2 ][ 2 ];
		mat[ 2 ][ 2 ] = tmp;
	}
}

int main()
{
	Matrix3f m, U, V;
	m[ 0 ][ 0 ] = 0.19377f;
	m[ 0 ][ 1 ] = 0.95348f;
	m[ 0 ][ 2 ] = 0.54060f;
 	m[ 1 ][ 0 ] = 0.93239f;
	m[ 1 ][ 1 ] = 0.73645f;
	m[ 1 ][ 2 ] = 0.53739f;
  	m[ 2 ][ 0 ] = 0.30896f;
	m[ 2 ][ 1 ] = 0.91772f;
	m[ 2 ][ 2 ] = 0.24861f;

	std::cout << m << std::endl << std::endl;


	svd( m, U, V );

	std::cout << U << std::endl << std::endl;
	std::cout << m << std::endl << std::endl;
	std::cout << V << std::endl << std::endl;

	Matrix3f x = U;
	x *= m;
	x *= V;

	std::cout << x << std::endl << std::endl;

/*
S =

  -0.561317   0.547568  -0.620558
  -0.651105  -0.755042  -0.077286
  -0.510867   0.360666   0.780343

U =

Diagonal Matrix

   1.89028         0         0
         0   0.57033         0
         0         0   0.20107

V =

  -0.462198  -0.852940   0.242623
  -0.784826   0.520821   0.335848
  -0.412821  -0.035188  -0.910132

   */
	return 0;
}
