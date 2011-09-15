#include "main.h"

#include <Eigen/Core>

void eigen_bench( const char* output )
{
	Eigen::VectorXf a, b;
	Time t;
	FILE* f;

	f = fopen( output, "wb" );

	for( size_t i = MINSIZE; i < MAXSIZE; i++ ) {
		size_t iter = Math::max( ( size_t ) ( MAXSIZE / i ), ( size_t ) MINLOOP );
		a.resize( i );
		b.resize( i );

		a.setZero();
		b.setZero();

		t.reset();
		for( size_t n = 0; n < iter; n++ ) {
			a += b * ALPHAF;
		}
		fprintf( f, "%zd %f\n", i, ( ( float ) ( i * sizeof( float ) * iter ) ) / ( 1.048576 * t.elapsedMicroSeconds() ) );
	}
	fclose( f );
}
