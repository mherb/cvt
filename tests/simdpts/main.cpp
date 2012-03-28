#include <cvt/math/Vector.h>
#include <cvt/math/Matrix.h>
#include <cvt/util/SIMDDebug.h>
#include <iostream>

#include <xmmintrin.h>
#include <emmintrin.h>

using namespace cvt;


void transform( Vector3f* dst, const Vector3f* src, const Matrix4f& _mat, size_t n )
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

		/* move data to from 3 Vector3f from the 4 homogenous coords with w == 1 */
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

int main()
{
	Matrix4f mat( 1.5f, 0.0f, 0.0f, 1.0f,
				  0.0f, 2.0f, 0.0f, 1.0f,
				  0.0f, 0.0f, 3.0f, 1.0f,
				  0.0f, 0.0f, 0.0f, 1.0f );
	Vector3f in[] = {
					Vector3f( 1.0f, 2.0f, 3.0f ),
					Vector3f( 4.0f, 5.0f, 6.0f ),
					Vector3f( 7.0f, 8.0f, 9.0f ),
					Vector3f( 10.0f, 11.0f, 12.0f )
				   };

	for( int i = 0; i < 4; i++ )
		std::cout << mat * in[ i ] << std::endl;

	Vector3f out[ 4 ];

	transform( out, in , mat, 4 );
	for( int i = 0; i < 4; i++ )
		std::cout << out[ i ] << std::endl;
}

