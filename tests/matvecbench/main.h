#ifndef MAIN_H
#define MAIN_H

#include <cvt/math/Math.h>
#include <cvt/util/Time.h>


#define MAXSIZE ( 50000 )
#define MINSIZE ( 4 )

#define MINLOOP 100


#define ALPHAF 123.45f

using namespace cvt;

void eigen_bench( const char* output );
void simd_bench( const char* output );
void cblas_bench( const char* output );
void template_bench( const char* output );
void rawc_bench( const char* output );

#endif
