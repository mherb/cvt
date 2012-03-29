#include <cvt/math/Vector.h>
#include <cvt/math/Matrix.h>
#include <cvt/util/SIMDDebug.h>
#include <iostream>

#include <xmmintrin.h>
#include <emmintrin.h>

using namespace cvt;

void transformPointsHomogenize( Vector3f* dst,const Matrix4f& _mat, const Vector3f* src, size_t n )
{
	__m128 mat[ 4 ], in1, in2, in3, tmp, out1, out2, out3, out4;
	mat[ 0 ] = _mm_loadu_ps( ( ( const float* ) _mat.ptr() ) );
	mat[ 1 ] = _mm_loadu_ps( ( ( const float* ) _mat.ptr() ) + 4 );
	mat[ 2 ] = _mm_loadu_ps( ( ( const float* ) _mat.ptr() ) + 8 );
	mat[ 3 ] = _mm_loadu_ps( ( ( const float* ) _mat.ptr() ) + 12 );

	_MM_TRANSPOSE4_PS( mat[ 0 ], mat[ 1 ], mat[ 2 ], mat[ 3 ] );

	size_t i = n >> 2; // 4 Vector3f make 12 floats ...
	while( i-- ){
		in1 = _mm_loadu_ps( ( ( const float* ) src ) + 0 );
		in2 = _mm_loadu_ps( ( ( const float* ) src ) + 4 );
		in3 = _mm_loadu_ps( ( ( const float* ) src ) + 8 );

#define MM_REPLICATE( xmm, pos ) ( __m128 ) _mm_shuffle_epi32( ( __m128i ) xmm, ( ( (pos) << 6) | ( ( pos ) << 4) | ( ( pos ) << 2) | ( pos ) ) )


//#define MM_REPLICATE( xmm, pos )  _mm_shuffle_ps( xmm, xmm, ( ( (pos) << 6) | ( ( pos ) << 4) | ( ( pos ) << 2) | ( pos ) ) )

		/* transform first Vector3f */
		tmp = MM_REPLICATE( in1, 0 );
		out1 = _mm_mul_ps( tmp, mat[ 0 ] );
		tmp = MM_REPLICATE( in1, 1 );
		out1 = _mm_add_ps( out1, _mm_mul_ps( tmp, mat[ 1 ] ) );
		tmp = MM_REPLICATE( in1, 2 );
		out1 = _mm_add_ps( out1, _mm_mul_ps( tmp, mat[ 2 ] ) );
		out1 = _mm_add_ps( out1, mat[ 3 ] );

		/* transform second Vector3f */
		tmp = MM_REPLICATE( in1, 3 );
		out2 = _mm_mul_ps( tmp, mat[ 0 ] );
		tmp = MM_REPLICATE( in2, 0 );
		out2 = _mm_add_ps( out2, _mm_mul_ps( tmp, mat[ 1 ] ) );
		tmp = MM_REPLICATE( in2, 1 );
		out2 = _mm_add_ps( out2, _mm_mul_ps( tmp, mat[ 2 ] ) );
		out2 = _mm_add_ps( out2, mat[ 3 ] );

		/* transform third Vector3f */
		tmp = MM_REPLICATE( in2, 2 );
		out3 = _mm_mul_ps( tmp, mat[ 0 ] );
		tmp = MM_REPLICATE( in2, 3 );
		out3 = _mm_add_ps( out3, _mm_mul_ps( tmp, mat[ 1 ] ) );
		tmp = MM_REPLICATE( in3, 0 );
		out3 = _mm_add_ps( out3, _mm_mul_ps( tmp, mat[ 2 ] ) );
		out3 = _mm_add_ps( out3, mat[ 3 ] );

		/* transform fourth Vector3f */
		tmp = MM_REPLICATE( in3, 1 );
		out4 = _mm_mul_ps( tmp, mat[ 0 ] );
		tmp = MM_REPLICATE( in3, 2 );
		out4 = _mm_add_ps( out4, _mm_mul_ps( tmp, mat[ 1 ] ) );
		tmp = MM_REPLICATE( in3, 3 );
		out4 = _mm_add_ps( out4, _mm_mul_ps( tmp, mat[ 2 ] ) );
		out4 = _mm_add_ps( out4, mat[ 3 ] );

		/* homogenous division */
		out1 = _mm_div_ps( out1, MM_REPLICATE( out1, 3 ) );
		out2 = _mm_div_ps( out2, MM_REPLICATE( out2, 3 ) );
		out3 = _mm_div_ps( out3, MM_REPLICATE( out3, 3 ) );
		out4 = _mm_div_ps( out4, MM_REPLICATE( out4, 3 ) );

		/* move data to 4 Vector3f from the 4 Vector3f with w == 1 */
		tmp = _mm_move_ss( _mm_shuffle_ps( out1, out1, 0x93 ), out2 );
		out1 = _mm_shuffle_ps( tmp, tmp, 0x39 );

		out2 = _mm_shuffle_ps( out2, out2, 0x39 );
		out2 = _mm_movelh_ps( out2, out3 );

		out3 = _mm_move_ss( _mm_shuffle_ps( out4, out4, 0x93 ), MM_REPLICATE( out3, 2 ) );

		/* store the result */
		_mm_storeu_ps( ( ( float* ) dst ) + 0, out1 );
		_mm_storeu_ps( ( ( float* ) dst ) + 4, out2 );
		_mm_storeu_ps( ( ( float* ) dst ) + 8, out3 );

		src += 4; // 4 Vector3f
		dst += 4;
	}

	i = n & 0x3;
	while( i-- )
		*dst++ = _mat * *src++;
}


void transformPoints( Vector3f* dst,const Matrix4f& _mat, const Vector3f* src, size_t n )
{
	__m128 mat[ 4 ], in1, in2, in3, tmp, out1, out2, out3, out4;
	mat[ 0 ] = _mm_loadu_ps( ( ( const float* ) _mat.ptr() ) );
	mat[ 1 ] = _mm_loadu_ps( ( ( const float* ) _mat.ptr() ) + 4 );
	mat[ 2 ] = _mm_loadu_ps( ( ( const float* ) _mat.ptr() ) + 8 );
	mat[ 3 ] = _mm_loadu_ps( ( ( const float* ) _mat.ptr() ) + 12 );

	_MM_TRANSPOSE4_PS( mat[ 0 ], mat[ 1 ], mat[ 2 ], mat[ 3 ] );

	size_t i = n >> 2; // 4 Vector3f make 12 floats ...
	while( i-- ){
		in1 = _mm_loadu_ps( ( ( const float* ) src ) + 0 );
		in2 = _mm_loadu_ps( ( ( const float* ) src ) + 4 );
		in3 = _mm_loadu_ps( ( ( const float* ) src ) + 8 );

#define MM_REPLICATE( xmm, pos ) ( __m128 ) _mm_shuffle_epi32( ( __m128i ) xmm, ( ( (pos) << 6) | ( ( pos ) << 4) | ( ( pos ) << 2) | ( pos ) ) )


//#define MM_REPLICATE( xmm, pos )  _mm_shuffle_ps( xmm, xmm, ( ( (pos) << 6) | ( ( pos ) << 4) | ( ( pos ) << 2) | ( pos ) ) )

		/* transform first Vector3f */
		tmp = MM_REPLICATE( in1, 0 );
		out1 = _mm_mul_ps( tmp, mat[ 0 ] );
		tmp = MM_REPLICATE( in1, 1 );
		out1 = _mm_add_ps( out1, _mm_mul_ps( tmp, mat[ 1 ] ) );
		tmp = MM_REPLICATE( in1, 2 );
		out1 = _mm_add_ps( out1, _mm_mul_ps( tmp, mat[ 2 ] ) );
		out1 = _mm_add_ps( out1, mat[ 3 ] );

		/* transform second Vector3f */
		tmp = MM_REPLICATE( in1, 3 );
		out2 = _mm_mul_ps( tmp, mat[ 0 ] );
		tmp = MM_REPLICATE( in2, 0 );
		out2 = _mm_add_ps( out2, _mm_mul_ps( tmp, mat[ 1 ] ) );
		tmp = MM_REPLICATE( in2, 1 );
		out2 = _mm_add_ps( out2, _mm_mul_ps( tmp, mat[ 2 ] ) );
		out2 = _mm_add_ps( out2, mat[ 3 ] );

		/* transform third Vector3f */
		tmp = MM_REPLICATE( in2, 2 );
		out3 = _mm_mul_ps( tmp, mat[ 0 ] );
		tmp = MM_REPLICATE( in2, 3 );
		out3 = _mm_add_ps( out3, _mm_mul_ps( tmp, mat[ 1 ] ) );
		tmp = MM_REPLICATE( in3, 0 );
		out3 = _mm_add_ps( out3, _mm_mul_ps( tmp, mat[ 2 ] ) );
		out3 = _mm_add_ps( out3, mat[ 3 ] );

		/* transform fourth Vector3f */
		tmp = MM_REPLICATE( in3, 1 );
		out4 = _mm_mul_ps( tmp, mat[ 0 ] );
		tmp = MM_REPLICATE( in3, 2 );
		out4 = _mm_add_ps( out4, _mm_mul_ps( tmp, mat[ 1 ] ) );
		tmp = MM_REPLICATE( in3, 3 );
		out4 = _mm_add_ps( out4, _mm_mul_ps( tmp, mat[ 2 ] ) );
		out4 = _mm_add_ps( out4, mat[ 3 ] );

		/* move data to 4 Vector3f from the 4 Vector3f with w == 1 */
		tmp = _mm_move_ss( _mm_shuffle_ps( out1, out1, 0x93 ), out2 );
		out1 = _mm_shuffle_ps( tmp, tmp, 0x39 );

		out2 = _mm_shuffle_ps( out2, out2, 0x39 );
		out2 = _mm_movelh_ps( out2, out3 );

		out3 = _mm_move_ss( _mm_shuffle_ps( out4, out4, 0x93 ), MM_REPLICATE( out3, 2 ) );

		/* store the result */
		_mm_storeu_ps( ( ( float* ) dst ) + 0, out1 );
		_mm_storeu_ps( ( ( float* ) dst ) + 4, out2 );
		_mm_storeu_ps( ( ( float* ) dst ) + 8, out3 );

		src += 4; // 4 Vector3f
		dst += 4;
	}

	i = n & 0x3;
	while( i-- )
		*dst++ = _mat * *src++;
}

void sumPoints( Vector3f& dst, const Vector3f* src, size_t n )
{
	__m128 result = _mm_setzero_ps();
	__m128 zero = _mm_setzero_ps();
	__m128 tmp;
	const __m128i mask = _mm_set_epi32( 0x0, 0x80808080, 0x80808080, 0x80808080 );

	size_t i = n >> 2; // 4 Vector3f make 12 floats ...
	while( i-- ){

		tmp = _mm_loadu_ps( ( ( const float* ) src ) + 0 );
		result = _mm_add_ps( result, tmp );
		result = _mm_add_ps( result, _mm_shuffle_ps( _mm_movehl_ps( tmp, zero ), zero, _MM_SHUFFLE( 0, 0, 0, 3 ) ) );

		tmp = _mm_loadu_ps( ( ( const float* ) src ) + 4 );
		result = _mm_add_ps( result, _mm_movehl_ps( zero, tmp ) );
		tmp = _mm_movelh_ps( zero, tmp );
		result = _mm_add_ps( result, _mm_shuffle_ps( tmp, tmp, 0x39 ) );

		tmp = _mm_loadu_ps( ( ( const float* ) src ) + 8 );
		result = _mm_add_ps( result, _mm_shuffle_ps( tmp, tmp, 0x39 ) );
		result = _mm_add_ps( result, _mm_shuffle_ps( zero, _mm_movelh_ps( tmp, zero ), _MM_SHUFFLE( 3, 0, 0, 0 ) ) );

		src += 4;
	}
	_mm_maskmoveu_si128( ( __m128i ) result, mask, ( char* ) dst.ptr() );

	i = n & 0x3;
	while( i-- )
		dst += *src++;
}

void sumPoints( Vector2f& dst, const Vector2f* src, size_t n )
{
	__m128 result = _mm_setzero_ps();
	__m128 zero = _mm_setzero_ps();
	__m128 tmp;
	const __m128i mask = _mm_set_epi32( 0x0, 0x0, 0x80808080, 0x80808080 );

	size_t i = n >> 1; // 4 Vector3f make 12 floats ...
	while( i-- ){

		tmp = _mm_loadu_ps( ( ( const float* ) src ) + 0 );
		result = _mm_add_ps( result, tmp );
		src += 2;
	}
	result = _mm_add_ps( result, _mm_movehl_ps( zero, result ) );
	_mm_maskmoveu_si128( ( __m128i ) result, mask, ( char* ) dst.ptr() );

	if( n & 0x1 )
		dst += *src;
}

void transformPoints( Vector2f* dst, const Matrix2f& _mat, const Vector2f* src, size_t n )
{
	__m128 mat[ 2 ], tmp, out;

	tmp = _mm_loadu_ps( _mat.ptr() );
	mat[ 0 ] = _mm_shuffle_ps( tmp, tmp, _MM_SHUFFLE( 2, 0, 2, 0 ) );
	mat[ 1 ] = _mm_shuffle_ps( tmp, tmp, _MM_SHUFFLE( 3, 1, 3, 1 ) );

	size_t i = n >> 1; // 2 Vector2f make 4 floats ...
	while( i-- ){
		tmp = _mm_loadu_ps( ( ( const float* ) src ) + 0 );
		out = _mm_mul_ps( _mm_shuffle_ps( tmp, tmp, _MM_SHUFFLE( 2, 2, 0, 0 ) ), mat[ 0 ] );
		out = _mm_add_ps( out, _mm_mul_ps( _mm_shuffle_ps( tmp, tmp, _MM_SHUFFLE( 3, 3, 1, 1 ) ), mat[ 1 ] ) );
		_mm_storeu_ps( ( float* ) dst, out );
		src += 2;
		dst += 2;
	}

	if( n & 0x01 )
		*dst = _mat * *src;
}


int main()
{
	Matrix2f mat( 1.0f, 0.0f,
				  0.0f, 2.0f );
	Vector2f in[] = {
					Vector2f( 1.0f, 2.0f ),
					Vector2f( 4.0f, 5.0f ),
					Vector2f( 7.0f, 8.0f ),
					Vector2f( 3.0f, 6.0f ),
					Vector2f( 5.0f, 4.0f )
				   };

	Vector2f out[ 5 ];
	transformPoints( out, mat, in, 5 );

	for( int i = 0; i < 5; i++ )
		std::cout << out[ i ] << std::endl;

	for( int i = 0; i < 5; i++ )
		std::cout << mat * in[ i ] << std::endl;
	return 0;

#if 0
	Vector3f sum;
	Matrix4f mat( 1.5f, 0.0f, 0.0f, 1.0f,
				  0.0f, 2.0f, 0.0f, 1.0f,
				  0.0f, 0.0f, 3.0f, 1.0f,
				  0.0f, 0.0f, 0.0f, 1.0f );
	Vector3f in[] = {
					Vector3f( 1.0f, 2.0f, 3.0f ),
					Vector3f( 4.0f, 5.0f, 6.0f ),
					Vector3f( 7.0f, 8.0f, 9.0f ),
					Vector3f( 10.0f, 11.0f, 12.0f ),
					Vector3f( 6.0f, 5.0f, 4.0f )
				   };


	sumPoints( sum, in, 5 );
	sum /= 5.0f;
	std::cout << sum << std::endl;

	sum.setZero();
	for( int i = 0; i < 5; i++ )
		sum += in[ i ];
	sum /= 5.0f;

	std::cout << sum << std::endl;
	return 0;

	for( int i = 0; i < 4; i++ )
		std::cout << mat * in[ i ] << std::endl;

	Vector3f out[ 4 ];

	transformPoints( out, mat, in, 4 );
	for( int i = 0; i < 4; i++ )
		std::cout << out[ i ] << std::endl;

#endif
}

