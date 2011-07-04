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

void svd2( Matrix2f& mat )
{
	float c, s;
	float t[ 4 ];
	Matrix2f v;
	int i = 0, k = 1;

	std::cout << mat << std::endl;

	/* make 2 x 2 symmetric */
	Math::givens( c, s, mat[ i ][ i ] + mat[ k ][ k ], mat[ k ][ i ] - mat[ i ][ k ] );
	t[ 0 ] = mat[ i ][ i ] * c - mat[ i ][ k ] * s;
	t[ 1 ] = mat[ i ][ i ] * s + mat[ i ][ k ] * c;
	t[ 2 ] = mat[ k ][ i ] * c - mat[ k ][ k ] * s;
	t[ 3 ] = mat[ k ][ i ] * s + mat[ k ][ k ] * c;
	mat[ i ][ i ] = t[ 0 ];
	mat[ i ][ k ] = t[ 1 ];
	mat[ k ][ i ] = t[ 2 ];
	mat[ k ][ k ] = t[ 3 ];

	std::cout << mat << std::endl;

	v.identity();
	/* apply the givens rotation to v */
	t[ 0 ] = v[ i ][ i ] * c + v[ k ][ i ] * s;
	t[ 1 ] = v[ i ][ k ] * c + v[ k ][ k ] * s;
	t[ 2 ] = - v[ i ][ i ] * s + v[ k ][ i ] * c;
	t[ 3 ] = - v[ i ][ k ] * s + v[ k ][ k ] * c;
	v[ i ][ i ] = t[ 0 ];
	v[ i ][ k ] = t[ 1 ];
	v[ k ][ i ] = t[ 2 ];
	v[ k ][ k ] = t[ 3 ];

	std::cout << v << std::endl;

	/* make 2 x 2 diagonal */
			Math::jacobi( c, s, mat[ i ][ i ], mat[ k ][ i ], mat[ k ][ k ] );

			std::cout << c << " " << s << std::endl;
			jacobi_cs( mat[ i ][ i ], mat[ k ][ i ], mat[ k ][ k ] );
			t[ 0 ] = mat[ i ][ i ] * c - mat[ i ][ k ] * s;
			t[ 1 ] = mat[ i ][ i ] * s + mat[ i ][ k ] * c;
			t[ 2 ] = mat[ k ][ i ] * c - mat[ k ][ k ] * s;
			t[ 3 ] = mat[ k ][ i ] * s + mat[ k ][ k ] * c;
			mat[ i ][ i ] = t[ 0 ];
			mat[ i ][ k ] = t[ 1 ];
			mat[ k ][ i ] = t[ 2 ];
			mat[ k ][ k ] = t[ 3 ];

	std::cout << mat << std::endl;
/*
   jacobi^T 2x2 jacobi =
   matrix([ -(c+b)*cos*sin+(a-d)*cos^2+d, (a-d)*cos*sin+(c+b)*cos^2-c],
		  [  (a-d)*cos*sin+(c+b)*cos^2-b, (c+b)*cos*sin-(a-d)*cos^2+a] )

 */
				/* apply the jacobi rotation to V */
			t[ 0 ] = v[ i ][ i ] * c + v[ k ][ i ] * s;
			t[ 1 ] = v[ i ][ k ] * c + v[ k ][ k ] * s;
			t[ 2 ] = - v[ i ][ i ] * s + v[ k ][ i ] * c;
			t[ 3 ] = - v[ i ][ k ] * s + v[ k ][ k ] * c;
			v[ i ][ i ] = t[ 0 ];
			v[ i ][ k ] = t[ 1 ];
			v[ k ][ i ] = t[ 2 ];
			v[ k ][ k ] = t[ 3 ];

	std::cout << v << std::endl;
}

void svd( Matrix3f& mat, Matrix3f& u, Matrix3f& v )
{
	float c, s;
	float t[ 4 ];
	u.identity();
	v.identity();
	
	for( int i = 0; i < 3; i++ ) {
		for( int k = i + 1; k < 3; k++ ) {
			/* make 2 x 2 symmetric */
			Math::givens( c, s, mat[ i ][ i ] + mat[ k ][ k ], mat[ k ][ i ] - mat[ i ][ k ] );
			t[ 0 ] = mat[ i ][ i ] * c - mat[ i ][ k ] * s;
			t[ 1 ] = mat[ i ][ i ] * s + mat[ i ][ k ] * c;
			t[ 2 ] = mat[ k ][ i ] * c - mat[ k ][ k ] * s;
			t[ 3 ] = mat[ k ][ i ] * s + mat[ k ][ k ] * c;
			mat[ i ][ i ] = t[ 0 ];
			mat[ i ][ k ] = t[ 1 ];
			mat[ k ][ i ] = t[ 2 ];
			mat[ k ][ k ] = t[ 3 ];

			/* apply the givens rotation to v */
			t[ 0 ] = v[ i ][ i ] * c + v[ k ][ i ] * s;
			t[ 1 ] = v[ i ][ k ] * c + v[ k ][ k ] * s;
			t[ 2 ] = - v[ i ][ i ] * s + v[ k ][ i ] * c;
			t[ 3 ] = - v[ i ][ k ] * s + v[ k ][ k ] * c;
			v[ i ][ i ] = t[ 0 ];
			v[ i ][ k ] = t[ 1 ];
			v[ k ][ i ] = t[ 2 ];
			v[ k ][ k ] = t[ 3 ];

			/* make 2 x 2 diagonal */
			Math::jacobi( c, s, mat[ i ][ i ], mat[ i ][ k ], mat[ k ][ k ] );
			t[ 0 ] = mat[ i ][ i ] * c - mat[ i ][ k ] * s;
			t[ 1 ] = mat[ i ][ i ] * s + mat[ i ][ k ] * c;
			t[ 2 ] = mat[ k ][ i ] * c - mat[ k ][ k ] * s;
			t[ 3 ] = mat[ k ][ i ] * s + mat[ k ][ k ] * c;
			mat[ i ][ i ] = t[ 0 ];
			mat[ i ][ k ] = t[ 1 ];
			mat[ k ][ i ] = t[ 2 ];
			mat[ k ][ k ] = t[ 3 ];

			/* apply the transposed jacobi rotation to U */
			t[ 0 ] = u[ i ][ i ] * c + u[ k ][ i ] * s;
			t[ 1 ] = u[ i ][ k ] * c + u[ k ][ k ] * s;
			t[ 2 ] = - u[ i ][ i ] * s + u[ k ][ i ] * c;
			t[ 3 ] = - u[ i ][ k ] * s + u[ k ][ k ] * c;
			u[ i ][ i ] = t[ 0 ];
			u[ i ][ k ] = t[ 1 ];
			u[ k ][ i ] = t[ 2 ];
			u[ k ][ k ] = t[ 3 ];

			/* apply the jacobi rotation to V */
			t[ 0 ] = v[ i ][ i ] * c + v[ k ][ i ] * s;
			t[ 1 ] = v[ i ][ k ] * c + v[ k ][ k ] * s;
			t[ 2 ] = - v[ i ][ i ] * s + v[ k ][ i ] * c;
			t[ 3 ] = - v[ i ][ k ] * s + v[ k ][ k ] * c;
			v[ i ][ i ] = t[ 0 ];
			v[ i ][ k ] = t[ 1 ];
			v[ k ][ i ] = t[ 2 ];
			v[ k ][ k ] = t[ 3 ];
		}
	}


}

int main()
{
	Matrix2f m2;
	m2[ 0 ][ 0 ] = 4;
	m2[ 0 ][ 1 ] = 3;
	m2[ 1 ][ 0 ] = 2;
	m2[ 1 ][ 1 ] = 1;

	svd2( m2 );
	return 0;



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

	svd( m, U, V );

	std::cout << U << std::endl;
	std::cout << m << std::endl;
	std::cout << V << std::endl;

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
