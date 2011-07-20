#ifndef CVTMATH_H
#define CVTMATH_H

#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include <float.h>
#include <iostream>

#include <cvt/util/Exception.h>

#include <Eigen/Core>
#include <Eigen/LU>

namespace cvt {

	namespace Math {

		typedef union {
			uint32_t i;
			float f;
		} _flint32;

		typedef union {
			uint64_t i;
			double d;
		} _flint64;

		static const float  PI	     = 3.14159265358979323846f;
		static const float  TWO_PI   = 2.0f * PI;
		static const float  HALF_PI  = 0.5f * PI;
		static const float  E	     = 2.71828182845904523536f;
		static const float  EPSILONF = FLT_EPSILON;
		static const float  MAXF	 = FLT_MAX;
		static const double EPSILOND = DBL_EPSILON;
		static const double MAXD	 = DBL_MAX;

		template<class T> static inline T max( T x, T y ) { return ( x > y ) ? x : y; }
		template<class T> static inline T min( T x, T y ) { return ( x < y ) ? x : y; }
		template<class T> static inline size_t maxIndex( T x, T y ) { return  ( x > y ) ? 0 : 1; }
		template<class T> static inline size_t minIndex( T x, T y ) { return ( x < y ) ? 0 : 1; }

		template<typename T> static inline T mix( T a, T b, float alpha ) { return a + ( b - a ) * alpha; }
		template<typename T> static inline T mix( T a, T b, double alpha ) { return a + ( b - a ) * alpha; }

		template<typename T> static inline T clamp( T v, T min, T max ) { return ( v < min ) ? min : ( v > max ) ? max : v; }

		static inline size_t pad16( size_t v ) { return v & 0xf ?  ( ( v | 0xf ) + 1 ) : v; }

		static inline size_t pad( size_t v, size_t multiple )
		{
			size_t r = v % multiple;
			if( !r )
				return v;
			return v + multiple - r;
		}

		template<typename T> static inline T sqr( T v ) { return v * v; }

		size_t solveQuadratic( float a, float b, float c, float (&result)[ 2 ] );
		size_t solveQuadratic( double a, double b, double c, double (&result)[ 2 ] );

		static inline float deg2Rad( float a )
		{
			return a * PI / 180.0f;
		}

		static inline float rad2Deg( float a )
		{
			return a * 180.0f / PI;
		}

		static inline float sqrt( float f )
		{
			return ::sqrtf( f );
		}

		static inline double sqrt( double d )
		{
			return ::sqrt( d );
		}

		template <typename T>
		static inline float sqrt( T f )
		{
			return ::sqrtf( ( float )f );
		}

		static inline float invSqrt( float f )
		{
			return 1.0f / ::sqrtf( f );
		}

		static inline double invSqrt( double d )
		{
			return 1.0 / ::sqrt( d );
		}

		static inline float abs( float f )
		{
			_flint32 u;
			u.f = f;
			u.i &= 0x7FFFFFFF;
			return u.f;
		}

		static inline double abs( double d )
		{
			_flint64 u;
			u.d = d;
			u.i &= 0x7FFFFFFFFFFFFFFFULL;
			return u.d;
		}

		static inline int abs( int i )
		{
			return ::abs( i );
		}

		static inline float cos( float x )
		{
			return ::cosf( x );
		}

		static inline double cos( double x )
		{
			return ::cos( x );
		}

		static inline float sin( float x )
		{
			return ::sinf( x );
		}

		static inline double sin( double x )
		{
			return ::sin( x );
		}

		static inline float acos( float x )
		{
			return ::acosf( x );
		}

		static inline double acos( double x )
		{
			return ::acos( x );
		}

		static inline float asin( float x )
		{
			return ::asinf( x );
		}

		static inline double asin( double x )
		{
			return ::asin( x );
		}
/*
		static inline void sincos( float rad, float& sin, float& cos )
		{
			::sincosf( rad, &sin, &cos );
		}

		static inline void sincos( double rad, double& sin, double& cos )
		{
			::sincos( rad, &sin, &cos );
		}
*/
		static inline float round( float x )
		{
			return ::roundf( x );
		}

		static inline float round( double x )
		{
			return ::round( x );
		}

		static inline float ceil( float x )
		{
			return ::ceilf( x );
		}

		static inline double ceil( double x )
		{
			return ::ceil( x );
		}

		static inline float floor( float x )
		{
			return ::floorf( x );
		}

		static inline double floor( double x )
		{
			return ::floor( x );
		}

		static inline float exp( float x )
		{
			return ::expf( x );
		}

		static inline double exp( double x )
		{
			return ::exp( x );
		}

        static inline float log( float x )
        {
            return ::logf( x );
        }

        static inline double log( double x )
        {
            return ::log( x );
        }

		static inline float tan( float x )
		{
			return ::tanf( x );
		}

		static inline double tan( double x )
		{
			return ::tan( x );
		}

		static inline float atan2( float y, float x )
		{
			return ::atan2f( y, x );
		}

		static inline double atan2( double y, double x )
		{
			return ::atan2( y, x );
		}

		static inline double pow( double x, double y )
		{
			return ::pow( x, y );
		}

		static inline float pow( float x, float y )
		{
			return ::powf( x, y );
		}


		static inline ssize_t rand()
		{
			return ( ssize_t ) ::random();
		}

		static inline int rand( int min, int max )
		{
			return min + ( ( max - min ) * ( ( float ) ::random() / ( ( size_t ) RAND_MAX + 1 )  ) );
		}

		static inline float rand( float min, float max )
		{
			return min + ( ( max - min ) * ( ( float ) ::random() / ( ( float ) RAND_MAX + 1.0f )  ) );
		}

		static inline double rand( double min, double max )
		{
			return min + ( ( max - min ) * ( ( double ) ::random() / ( ( double ) RAND_MAX + 1.0 )  ) );
		}

		static inline float copysign( float dst, float src )
		{
			_flint32 _dst;
			_flint32 _src;

			_dst.f = dst;
			_src.f = src;
			_dst.i = ( _dst.i & ~( 1 << 31 ) ) | ( _src.i & ( 1 << 31 ) );
			return _dst.f;
		}

		static inline double copysign( double dst, double src )
		{
			_flint64 _dst;
			_flint64 _src;

			_dst.d = dst;
			_src.d = src;
			_dst.i = ( _dst.i & ~( ( uint64_t ) 1 << 63 ) ) | ( _src.i & ( ( uint64_t ) 1 << 63 ) );
			return _dst.d;
		}

		template<typename T>
		static inline T sgn( T x )
		{
			return ( x > 0 ) ? 1 : ( ( x < 0 ) ? -1 : 0 );
		}

		template<>
		inline float sgn<float>( float x )
		{
			return copysign( 1.0f, x );
		}

		template<>
		inline double sgn<double>( double x )
		{
			return copysign( 1.0, x );
		}

		static inline float givens( float& c, float& s, float a, float b )
		{
			float t, u;
			if( b == 0 ) {
				c = copysign( 1.0f, a );
				s = 0;
				return abs(a);
			} else if( a == 0) {
				c = 0;
				s = copysign( 1.0f, b );
				return abs(b);
			} else if( abs( b ) > abs( a ) ) {
				t = a / b;
				u = copysign( sqrt( 1.0f + sqr( t ) ), b );
				s = 1.0f / u;
				c = s * t;
				return b * u;
			} else {
				t = b / a;
				u = copysign( sqrt( 1.0f + sqr( t ) ), a );
				c = 1.0f / u;
				s = c * t;
				return a * u;
			}
		}

		static inline double givens( double& c, double& s, double a, double b )
		{
			double t, u;
			if( b == 0 ) {
				c = copysign( 1.0, a );
				s = 0;
				return abs(a);
			} else if( a == 0) {
				c = 0;
				s = copysign( 1.0, b );
				return abs(b);
			} else if( abs( b ) > abs( a ) ) {
				t = a / b;
				u = copysign( sqrt( 1.0 + sqr( t ) ), b );
				s = 1.0 / u;
				c = s * t;
				return b * u;
			} else {
				t = b / a;
				u = copysign( sqrt( 1.0 + sqr( t ) ), a );
				c = 1.0 / u;
				s = c * t;
				return a * u;
			}
		}

		static inline void jacobi( float& c, float& s, float x, float y, float z )
		{
			float t, u;
			if( y == 0 ) {
				c = copysign( 1.0f, x );
				s = 0;
			} else {
				float tau = ( z - x ) / ( 2.0f * y );
				t = copysign( 1.0f / ( abs( tau ) + sqrt( 1.0f + sqr( tau ) ) ) , tau );
				u = sqrt( 1.0f + sqr( t ) );
				c = 1.0f / u;
				s = c * t;
			}
		}


		static inline void jacobi( double& c, double& s, double x, double y, double z )
		{
			double t, u;
			if( y == 0 ) {
				c = copysign( 1.0, x );
				s = 0;
			} else {
				double tau = ( z - x ) / ( 2.0 * y );
				t = copysign( 1.0 / ( abs( tau ) + sqrt( 1.0 + sqr( tau ) ) ) , tau );
				u = sqrt( 1.0 + sqr( t ) );
				c = 1.0 / u;
				s = c * t;
			}
		}

		/*
		   The famous fast inverse square root approximation found in the code
		   of id-tech 3/4.
		 */
		static inline float fastInvSqrtf( float f )
		{
			_flint32 u;
			float fhalf;

			fhalf = 0.5f * f;
			u.f = f;
			u.i = 0x5F3759DF - ( u.i >> 1 );
			f = u.f;
			return f * ( 1.5f - fhalf * f * f );
		}

		/* Round to nearest integer */
		static inline int rint( float f ) {
			int i;
			__asm__ __volatile__ ("fistpl %0" : "=m" (i) : "t" (f) : "st") ;
			return i;
		}

		template<typename T>
		static inline size_t gcd( T x, T y )
		{
			T remainder;

			while ( ( remainder = x % y ) != 0 ) {
				x = y;
				y = remainder;
			}
			return y;
		}

		/*
		 * Algorithm from Goloub, v. Loan p.573f
		 * but with fixed number of pade approximants;
		 * 6 approximants give precision 3.39452e-16
		 */
		template<class Matrix>
		static inline void exponential( const Matrix & A, Matrix & result, size_t padeApprox = 6 )
		{
			double infNorm = 0.0;

			for( int r = 0; r < A.rows(); r++ ){
				double rowSum = 0.0;
				for( int c = 0; c < A.cols(); c++ ){
					rowSum += A( r, c );
				}
				if( rowSum > infNorm )
					infNorm = rowSum;
			}

			int j = max( 0, 1 + int( log( infNorm ) / log( 2.0 ) ) );

			// tmpA = A * 2^j
			Matrix tmpA = A / ( 1 << j ) ;

			Matrix D = Matrix::Identity();
			Matrix N = Matrix::Identity();
			Matrix X = Matrix::Identity();
			Matrix cX;

			double c = 1.0;
			double s = -1.0;
			size_t q = padeApprox;
			size_t twoq = (padeApprox << 1);

			for( size_t k = 1; k < padeApprox; ++k ){
				c *= q / ( double )(twoq * k);
				X = tmpA * X;
				cX = X*c;
				N += cX;
				D += ( s*cX );
				--q;
				--twoq;
				s *= -1;
			}

			result = D.inverse();
			result *= N;

			for( int k = 0; k < j; ++k )
				result = result * result;
		}
	}
}

#endif
