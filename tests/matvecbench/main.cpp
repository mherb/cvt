#include <iostream>
#include "main.h"

int main()
{
	Time t;
	t.reset();
	eigen_bench( "eigen.txt" );
	printf("%f ms\n", t.elapsedMilliSeconds() );
	t.reset();
	simd_bench( "simd.txt" );
	printf("%f ms\n", t.elapsedMilliSeconds() );
	t.reset();
	cblas_bench( "cblas.txt" );
	printf("%f ms\n", t.elapsedMilliSeconds() );
	t.reset();
	template_bench( "template.txt" );
	printf("%f ms\n", t.elapsedMilliSeconds() );
	t.reset();
	rawc_bench("rawc.txt");
	printf("%f ms\n", t.elapsedMilliSeconds() );
}
