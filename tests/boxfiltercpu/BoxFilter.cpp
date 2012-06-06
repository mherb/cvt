#include "BoxFilter.h"
#include "Utils.h"

#include <xmmintrin.h>
#include <cvt/gfx/IMapScoped.h>

using namespace cvt;

#define DO_SIMD_ONCE()												\
																	\
	/* once */														\
	r1 = _mm_loadu_ps( A );											\
	r2 = _mm_load_ps( B );											\
	r3 = _mm_load_ps( C );											\
	r4 = _mm_loadu_ps( D );											\
																	\
	r1 = _mm_sub_ps( r1, r2 ); 	/* r1 = a - b */					\
	r1 = _mm_add_ps( r1, r3 ); 	/* r1 = a - b + c */				\
	r1 = _mm_sub_ps( r1, r4 ); 	/* r1 = a - b + c - d */			\
	r1 = _mm_mul_ps( r1, r5 ); 	/* r1 = (a - b + c - d) / scale; */	\
																	\
	_mm_storeu_ps(dst, r1);											\
																	\
	dst += 4;														\
	A += 4;															\
	B += 4;															\
	C += 4;															\
	D += 4;															\

template<bool simd>
void boxFilterLineInternal( float *dst, const float *A, const float *B, const float *C, const float *D, float scale, size_t n );

template<>
void boxFilterLineInternal<false>( float *dst, const float *A, const float *B, const float *C, const float *D, float scale, size_t n )
{
	while ( n-- )
	{
		float a = *A++;
		float b = *B++;
		float c = *C++;
		float d = *D++;

		*dst++ = (a-b-d+c)*scale;
	}
}

template<>
void boxFilterLineInternal<true>( float *dst, const float *A, const float *B, const float *C, const float *D, float scale, size_t n )
{
	// align the first two pointer to address multiple of 16 bytes
	while ( size_t(B) & 0x0F )
	{
		float a = *A++;
		float b = *B++;
		float c = *C++;
		float d = *D++;

		*dst++ = ( a-b-d+c )*scale;
		--n;
	}

	__m128 r1, r2, r3, r4;
	const __m128 r5 = _mm_set1_ps( scale );

	// loop unroll: process 8 blocks of 4 floats at a time
	size_t i = n >> 5;
	while( i-- ) {
		DO_SIMD_ONCE();
		DO_SIMD_ONCE();
		DO_SIMD_ONCE();
		DO_SIMD_ONCE();

		DO_SIMD_ONCE();
		DO_SIMD_ONCE();
		DO_SIMD_ONCE();
		DO_SIMD_ONCE();
	}

	// process up to 8 blocks of 4 floats
	i = (n >> 3) & 0x07;
	while ( i-- ) {
		DO_SIMD_ONCE();
	}

	// process up to 4 floats
	i = n & 0x03;
	while ( i-- )
	{
		float a = *A++;
		float b = *B++;
		float c = *C++;
		float d = *D++;

		*dst++ = ( a-b-d+c ) * scale;
	}
}

template<bool simd>
void boxFilterLine( float *pdst, const float *psrc, int line, size_t stride, int height, int width, int boxWidth, int boxHeight )
{
	// process first boxWidth/2 of line
	int x = 0;

	int left 	= x - 1 - ( boxWidth / 2 );
	int right 	= x - 1 + ( boxWidth / 2 + 1 );
	int top 	= line - 1 - ( boxHeight / 2 );
	int bottom 	= line - 1 + ( boxHeight / 2 + 1);

	float a, b, c, d;

	if ( top < 0 )
	{
		top = 0;
	}

	if ( bottom > height - 1 )
	{
		bottom	= height - 1;
	}

	const float *psrc1 = linePtr( psrc, stride, top );
	const float *psrc2 = linePtr( psrc, stride, bottom );

	while ( left < 0 )
	{
		a = psrc2[ right ];
		d = psrc1[ right ];

		pdst[ x ] = ( a - d ) / ( (right + 1) * ( bottom - top) );

		++ x;

		left  = x - 1 - ( boxWidth / 2 );
		right = x - 1 + ( boxWidth / 2 + 1 );
	}

	// process internal line
	boxFilterLineInternal<simd>( &pdst[ x ], &psrc2[ right ], &psrc2[ left ], &psrc1[ left ], &psrc1[ right ], 1.0 / ( ( right - left )*( bottom-top ) ), width - boxWidth );

	// process last boxWidth/2 of line
	x = width - boxWidth / 2;

	left = x - 1 - ( boxWidth / 2 );
	right = width - 1;

	while ( left < width - 1 )
	{
		a = psrc2[ right ];
		b = psrc2[ left ];
		c = psrc1[ left ];
		d = psrc1[ right ];

		pdst[ x ] = ( a - b - d + c ) / ( ( right - left ) * ( bottom - top ) );

		++ x;

		left = x - 1 - ( boxWidth / 2 );
	}
}

void BoxFilter( cvt::Image &dst, const cvt::Image &src, const cvt::Vector2i &size )
{
	dst.reallocate( src );

	cvt::IMapScoped<float> mapdst( dst );
	cvt::IMapScoped<const float> mapsrc( src );

	size_t width = dst.width();
	size_t height = dst.height();

	int boxWidth = size.x;
	int boxHeight = size.y;

	const float *psrc = mapsrc.ptr();
	size_t stride = mapsrc.stride();

	SIMDType type = SIMD::instance()->type();
	bool simd = false;// type >= SIMDType(SIMD_SSE);

	mapdst.setLine( 0 );
	mapsrc.setLine( 0 );

	if ( !simd )
	{
		for ( int line = 0; line < height; line++ )
		{
			float* pdst = mapdst.ptr();

			boxFilterLine<true>( pdst, psrc, line, stride, height, width, boxHeight, boxWidth );

			mapdst++;
		}
	}
	else
	{
		for ( int line = 0; line < height; line++ )
		{
			float* pdst = mapdst.ptr();

			boxFilterLine<false>( pdst, psrc, line, stride, height, width, boxHeight, boxWidth );

			mapdst++;
		}
	}
}

