#include <iostream>
#include "main.h"

int main()
{
	eigen_bench( "eigen.txt" );
	simd_bench( "simd.txt" );
	cblas_bench( "cblas.txt" );
	template_bench( "template.txt" );
	rawc_bench("rawc.txt");
}
