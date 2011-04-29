#include <cvt/math/BLAS.h>
#include <cvt/math/Math.h>

namespace cvt
{
	/* LEVEL 1 */

	/* single precision, float*/
	void BLAS::srotg( float* a, float* b, float* c, float* s )
	{}

	void BLAS::srotmg( float* d1, float* d2, float* b1, const float b2, float* P )
	{}

	void BLAS::srot( const int N, float* X, const int incX, float* Y, const int incY, const float c, const float s )
	{}

	void BLAS::srotm( const int N, float* X, const int incX, float* Y, const int incY, const float* P )
	{}

	void BLAS::sswap( const int N, float* X, const int incX, float* Y, const int incY )
	{
		int n = N;
		float t;
		while( n-- ) {
			t = *X;
			*X = *Y;
			*Y = t;
			X += incX;
			Y += incY;
		}
	}

	void BLAS::sscal( const int N, const float alpha, float* X, const int incX )
	{
		int n = N;

		if( incX == 1 ) {
			while( n-- ) {
				*X = alpha * *X;
				X++
			}
		} else {
			while( n-- ) {
				*X = alpha * *X;
				X += incX;
			}

		}
	}

	void BLAS::scopy( const int N, const float* X, const int incX, float* Y, const int incY )
	{
		int n = N;

		if( incX == 1 ) {
			/* FIXME: use SIMD::Memcpy */
			while( n-- ) {
				*Y++ = *X++;
			}
		} else {
			while( n-- ) {
				*Y = *X;
				X += incX;
				Y += incY;
			}
		}
	}

	void BLAS::saxpy( const int N, const float alpha, const float* X, const int incX, float* Y, const int incY )
	{
		int n = N;

		if( incX == 1 && incY == 1 ) {
			while( n-- ) {
				*Y = alpha * *X++ + *Y;
				Y++;
			}
		} else {
			while( n-- ) {
				*Y = alpha * *X + *Y;
				X += incX;
				Y += incY;
			}
		}
	}

	float BLAS::sdot( const int N, const float* X, const int incX, const float* Y, const int incY )
	{
		float ret = 0;
		int n = N;

		if( incX == 1 && incY == 1 ) {
			while( n-- )
				ret += *X++ **Y++;
		} else {
			while( n-- ) {
				ret += *X * *Y;
				X += incX;
				X += incY;
			}
		}
		return ret;
	}

	float BLAS::sdsdot( const int N, const float alpha, const float* X, const int incX, const float* Y, const int incY )
	{
		float ret = 0;
		int n = N;

		if( incX == 1 && incY == 1 ) {
			while( n-- )
				ret += *X++ **Y++;
		} else {
			while( n-- ) {
				ret += *X * *Y;
				X += incX;
				X += incY;
			}
		}
		return ret + alpha;
	}

	float BLAS::snrm2( const int N, const float* X, const int incX )
	{
		float ret = 0;
		int n = N;

		if( incX == 1 ) {
			while( n-- ) {
				ret += *X * *X;
				X++;
			}
		} else {
			while( n-- ) {
				ret += *X * *X;
				X += incX;
			}
		}
		return Math::sqrt( ret );
	}

	float BLAS::sasum( const int N, const float* X, const int incX )
	{
		float ret = 0;
		int n = N;

		if( incX == 1 ) {
			while( n-- )
				ret += *X++;
		} else {
			while( n-- ) {
				ret += *X;
				X += incX;
			}
		}
		return ret;
	}

	int BLAS::isamax( const int N, const float* X, const int incX )
	{
		int ret = 0;
		float max = *X;

		for( int i = 1; i < N; i++, X += incX ) {
			if( *X > max ) {
				max = *X;
				ret = i;
			}
		}

		return ret;
	}

	/* LEVEL 2 */

	/* single precision, float*/

	void BLAS::sgemv( const BLAS_ORDER order, const BLAS_TRANSPOSE TransA, const int M, const int N, const float alpha, const float* A, const int lda, const float* X, const int incX, const float beta, float* Y, const int incY )
	{}

	void BLAS::gbmv( const BLAS_ORDER order, const BLAS_TRANSPOSE TransA, const int M, const int N, const int KL, const int KU, const float alpha, const float* A, const int lda, const float* X, const int incX, const float beta, float* Y, const int incY )
	{}

	void BLAS::ssymv( const BLAS_ORDER order, const BLAS_UPLO Uplo, const int N, const float alpha, const float* A, const int lda, const float* X, const int incX, const float beta, float* Y, const int incY )
	{}

	void BLAS::ssbmv( const BLAS_ORDER order, const BLAS_UPLO Uplo, const int N, const int K, const float alpha, const float* A, const int lda, const float* X, const int incX, const float beta, float* Y, const int incY )
	{}

	void BLAS::sspmv( const BLAS_ORDER order, const BLAS_UPLO Uplo, const int N, const float alpha, const float* Ap, const float* X, const int incX, const float beta, float* Y, const int incY )
	{}

	void BLAS::strmv( const BLAS_ORDER order, const BLAS_UPLO Uplo, const BLAS_TRANSPOSE TransA, const BLAS_DIAG Diag, const int N, const float* A, const int lda, float* X, const int incX )
	{}

	void BLAS::stbmv( const BLAS_ORDER order, const BLAS_UPLO Uplo, const BLAS_TRANSPOSE TransA, const BLAS_DIAG Diag, const int N, const int K, const float* A, const int lda, float* X, const int incX )
	{}

	void BLAS::stpmv( const BLAS_ORDER order, const BLAS_UPLO Uplo, const BLAS_TRANSPOSE TransA, const BLAS_DIAG Diag, const int N, const float* Ap, float* X, const int incX )
	{}

	void BLAS::strsv( const BLAS_ORDER order, const BLAS_UPLO Uplo, const BLAS_TRANSPOSE TransA, const BLAS_DIAG Diag, const int N, const float* A, const int lda, float* X, const int incX )
	{}

	void BLAS::stbsv( const BLAS_ORDER order, const BLAS_UPLO Uplo, const BLAS_TRANSPOSE TransA, const BLAS_DIAG Diag, const int N, const int K, const float* A, const int lda, float* X, const int incX )
	{}
	void BLAS::stpsv( const BLAS_ORDER order, const BLAS_UPLO Uplo, const BLAS_TRANSPOSE TransA, const BLAS_DIAG Diag, const int N, const float* Ap, float* X, const int incX )
	{}

	void BLAS::sger( const BLAS_ORDER order, const int M, const int N, const float alpha, const float* X, const int incX, const float* Y, const int incY, float* A, const int lda )
	{}

	void BLAS::ssyr( const BLAS_ORDER order, const BLAS_UPLO Uplo, const int N, const float alpha, const float* X, const int incX, float* A, const int lda )
	{}
	void BLAS::sspr( const BLAS_ORDER order, const BLAS_UPLO Uplo, const int N, const float alpha, const float* X, const int incX, float* Ap )
	{}

	void BLAS::ssyr2( const BLAS_ORDER order, const BLAS_UPLO Uplo, const int N, const float alpha, const float* X, const int incX, const float* Y, const int incY, float* A, const int lda )
	{}

	void BLAS::sspr2( const BLAS_ORDER order, const BLAS_UPLO Uplo, const int N, const float alpha, const float* X, const int incX, const float* Y, const int incY, float* A )
	{}

	/* LEVEL 3 */

	/* single precision, float*/
	void BLAS::sgemm( const BLAS_ORDER Order, const BLAS_TRANSPOSE TransA, const BLAS_TRANSPOSE TransB, const int M, const int N, const int K, const float alpha, const float* A, const int lda, const float* B, const int ldb, const float beta, float* C, const int ldc )
	{}

	void BLAS::ssymm( const BLAS_ORDER Order, const BLAS_SIDE Side, const BLAS_UPLO Uplo, const int M, const int N, const float alpha, const float* A, const int lda, const float* B, const int ldb, const float beta, float* C, const int ldc )
	{}

	void BLAS::ssyrk( const BLAS_ORDER Order, const BLAS_UPLO Uplo, const BLAS_TRANSPOSE Trans, const int N, const int K, const float alpha, const float* A, const int lda, const float beta, float* C, const int ldc )
	{}

	void BLAS::ssyr2k( const BLAS_ORDER Order, const BLAS_UPLO Uplo, const BLAS_TRANSPOSE Trans, const int N, const int K, const float alpha, const float* A, const int lda, const float* B, const int ldb, const float beta, float* C, const int ldc )
	{}

	void BLAS::strmm( const BLAS_ORDER Order, const BLAS_SIDE Side, const BLAS_UPLO Uplo, const BLAS_TRANSPOSE TransA, const BLAS_DIAG Diag, const int M, const int N, const float alpha, const float* A, const int lda, float* B, const int ldb )
	{}

	void BLAS::strsm( const BLAS_ORDER Order, const BLAS_SIDE Side, const BLAS_UPLO Uplo, const BLAS_TRANSPOSE TransA, const BLAS_DIAG Diag, const int M, const int N, const float alpha, const float* A, const int lda, float* B, const int ldb )
	{}

}
