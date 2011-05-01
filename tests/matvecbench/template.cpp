#include "main.h"

#include "Vector.h"
#include "VectorExpr.h"
#include "VectorScalarExpr.h"

void template_bench( const char* output )
{
	Time t;
	FILE* f;

	f = fopen( output, "wb" );

	for( size_t i = MINSIZE; i < MAXSIZE; i++ ) {
		size_t iter = Math::max( ( size_t ) ( MAXSIZE / i ), ( size_t ) MINLOOP );
		Vector<float> a( i );
		Vector<float> b( i );

		t.reset();
		for( size_t n = 0; n < iter; n++ ) {
			a = a + ALPHAF * b;
		}
		fprintf( f, "%zd %f\n", i, ( ( float ) ( i * sizeof( float ) * iter ) ) / ( 1.048576 * t.elapsedMicroSeconds() ) );
	}
	fclose( f );
}
