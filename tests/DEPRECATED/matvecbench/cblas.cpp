#include "main.h"
#include <stdlib.h>
extern "C" {
#ifdef APPLE
#include "/opt/local/include/cblas.h"
#else
#include <cblas.h>
#endif
}


static float* vec( size_t N )
{
	float* ptr;
	if( posix_memalign( ( void** ) &ptr, 16, sizeof( float ) * N ) ) {
		printf( "Memory error!\n" );
		exit( EXIT_FAILURE );
	}
	return ptr;
}

void cblas_bench( const char* output )
{
	Time t;
	FILE* f;

	f = fopen( output, "wb" );

	for( size_t i = MINSIZE; i < MAXSIZE; i++ ) {
		size_t iter = Math::max( ( size_t ) ( MAXSIZE / i ), ( size_t ) MINLOOP );
		float* a = vec( i );
		float* b = vec( i );

		memset( a, 0, sizeof( float ) * i );
		memset( b, 0, sizeof( float ) * i );

		t.reset();
		for( size_t n = 0; n < iter; n++ ) {
			cblas_saxpy( i, ALPHAF, b, 1, a, 1 );
		}
		fprintf( f, "%zd %f\n", i, ( ( float ) ( i * sizeof( float ) * iter ) ) / ( 1.048576 * t.elapsedMicroSeconds() ) );
		free( a );
		free( b );
	}
	fclose( f );
}
