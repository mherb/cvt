#ifndef CVT_BLAS_H
#define CVT_BLAS_H

namespace cvt {
	enum BLAS_ORDER { BLAS_ROW_MAJOR, BLAS_COL_MAJOR };
	enum BLAS_TRANSPOSE { BLAS_NOTRANS, BLAS_TRANS, BLAS_CONJTRANS };
	enum BLAS_UPLO { BLAS_UPPER, BLAS_LOWER };
	enum BLAS_DIAG { BLAS_NOUNIT, BLAS_UNIT };
	enum BLAS_SIDE { BLAS_LEFT, BLAS_RIGHT };

	class BLAS {
		/* LEVEL 1 */

		/* single precision, float*/
		virtual void	srotg(float *a, float *b, float *c, float *s);
		virtual void	srotmg(float *d1, float *d2, float *b1, const float b2, float *P) ;
		virtual void	srot(const int N, float *X, const int incX, float *Y, const int incY, const float c, const float s);
		virtual void	srotm(const int N, float *X, const int incX, float *Y, const int incY, const float *P);

		virtual void	sswap(const int N, float *X, const int incX, float *Y, const int incY);
		virtual void	sscal(const int N, const float alpha, float *X, const int incX);
		virtual void	scopy(const int N, const float *X, const int incX, float *Y, const int incY);
		virtual void	saxpy(const int N, const float alpha, const float *X, const int incX, float *Y, const int incY);

		virtual float   sdot(const int N, const float  *X, const int incX, const float  *Y, const int incY);

		virtual float   sdsdot(const int N, const float alpha, const float *X, const int incX, const float *Y, const int incY);

		virtual float   snrm2(const int N, const float *X, const int incX);
		virtual float   sasum(const int N, const float *X, const int incX);
		virtual int		isamax(const int N, const float  *X, const int incX);

		/* LEVEL 2 */

		/* single precision, float*/

		virtual void	sgemv( const BLAS_ORDER order, const BLAS_TRANSPOSE TransA, const int M, const int N,
							   const float alpha, const float *A, const int lda,
							   const float *X, const int incX, const float beta,
							   float *Y, const int incY);
		virtual void	gbmv( const BLAS_ORDER order,
							  const BLAS_TRANSPOSE TransA, const int M, const int N,
							  const int KL, const int KU, const float alpha,
							  const float *A, const int lda, const float *X,
							  const int incX, const float beta, float *Y, const int incY);

		virtual void	ssymv( const BLAS_ORDER order, const BLAS_UPLO Uplo,
							   const int N, const float alpha, const float *A,
							   const int lda, const float *X, const int incX,
							   const float beta, float *Y, const int incY);
		virtual void	ssbmv( const BLAS_ORDER order, const BLAS_UPLO Uplo,
							   const int N, const int K, const float alpha, const float *A,
							   const int lda, const float *X, const int incX,
							   const float beta, float *Y, const int incY);
		virtual void	sspmv( const BLAS_ORDER order, const BLAS_UPLO Uplo,
							   const int N, const float alpha, const float *Ap,
							   const float *X, const int incX,
							   const float beta, float *Y, const int incY);
		virtual void	strmv( const BLAS_ORDER order, const BLAS_UPLO Uplo,
							   const BLAS_TRANSPOSE TransA, const BLAS_DIAG Diag,
							   const int N, const float *A, const int lda,
							   float *X, const int incX);
		virtual void	stbmv( const BLAS_ORDER order, const BLAS_UPLO Uplo,
							   const BLAS_TRANSPOSE TransA, const BLAS_DIAG Diag,
							   const int N, const int K, const float *A, const int lda,
							   float *X, const int incX);
		virtual void	stpmv( const BLAS_ORDER order, const BLAS_UPLO Uplo,
							   const BLAS_TRANSPOSE TransA, const BLAS_DIAG Diag,
							   const int N, const float *Ap, float *X, const int incX);
		virtual void	strsv( const BLAS_ORDER order, const BLAS_UPLO Uplo,
							   const BLAS_TRANSPOSE TransA, const BLAS_DIAG Diag,
							   const int N, const float *A, const int lda, float *X,
							   const int incX);
		virtual void	stbsv( const BLAS_ORDER order, const BLAS_UPLO Uplo,
							   const BLAS_TRANSPOSE TransA, const BLAS_DIAG Diag,
							   const int N, const int K, const float *A, const int lda,
							   float *X, const int incX);
		virtual void	stpsv( const BLAS_ORDER order, const BLAS_UPLO Uplo,
							   const BLAS_TRANSPOSE TransA, const BLAS_DIAG Diag,
							   const int N, const float *Ap, float *X, const int incX);

		virtual void	sger( const BLAS_ORDER order, const int M, const int N,
							  const float alpha, const float *X, const int incX,
					          const float *Y, const int incY, float *A, const int lda);
		virtual void	ssyr( const BLAS_ORDER order, const BLAS_UPLO Uplo,
							  const int N, const float alpha, const float *X,
							  const int incX, float *A, const int lda);
		virtual void	sspr( const BLAS_ORDER order, const BLAS_UPLO Uplo,
							  const int N, const float alpha, const float *X,
							  const int incX, float *Ap);
		virtual void	ssyr2( const BLAS_ORDER order, const BLAS_UPLO Uplo,
					           const int N, const float alpha, const float *X,
							   const int incX, const float *Y, const int incY, float *A,
						       const int lda);
		virtual void	sspr2( const BLAS_ORDER order, const BLAS_UPLO Uplo,
							   const int N, const float alpha, const float *X,
							   const int incX, const float *Y, const int incY, float *A);

		/* LEVEL 3 */

		/* single precision, float*/
		virtual void sgemm( const BLAS_ORDER Order, const BLAS_TRANSPOSE TransA,
							const BLAS_TRANSPOSE TransB, const int M, const int N,
							const int K, const float alpha, const float *A,
							const int lda, const float *B, const int ldb,
							const float beta, float *C, const int ldc);
		virtual void ssymm( const BLAS_ORDER Order, const BLAS_SIDE Side,
							const BLAS_UPLO Uplo, const int M, const int N,
							const float alpha, const float *A, const int lda,
							const float *B, const int ldb, const float beta,
							float *C, const int ldc);
		virtual void ssyrk( const BLAS_ORDER Order, const BLAS_UPLO Uplo,
							const BLAS_TRANSPOSE Trans, const int N, const int K,
							const float alpha, const float *A, const int lda,
							const float beta, float *C, const int ldc);
		virtual void ssyr2k( const BLAS_ORDER Order, const BLAS_UPLO Uplo,
							const BLAS_TRANSPOSE Trans, const int N, const int K,
							const float alpha, const float *A, const int lda,
							const float *B, const int ldb, const float beta,
							float *C, const int ldc);
		virtual void strmm( const BLAS_ORDER Order, const BLAS_SIDE Side,
							const BLAS_UPLO Uplo, const BLAS_TRANSPOSE TransA,
							const BLAS_DIAG Diag, const int M, const int N,
							const float alpha, const float *A, const int lda,
							float *B, const int ldb);
		virtual void strsm( const BLAS_ORDER Order, const BLAS_SIDE Side,
							const BLAS_UPLO Uplo, const BLAS_TRANSPOSE TransA,
							const BLAS_DIAG Diag, const int M, const int N,
							const float alpha, const float *A, const int lda,
							float *B, const int ldb);
}

#endif
