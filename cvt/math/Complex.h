#ifndef CVT_COMPLEX_H
#define CVT_COMPLEX_H

#include <cvt/math/Math.h>
#include <iostream>

namespace cvt {
	template<typename T>
		class Complex {
			public:
				Complex();
				Complex( T re, T im = 0 );
				Complex( const Complex<T>& cx );

				void set( T re, T im );
				void setZero();

				Complex<T>& operator=( const Complex<T>& cx );

				Complex<T> operator-() const;

				Complex<T>  operator*( const Complex<T>& cx ) const;
				Complex<T>  operator/( const Complex<T>& cx ) const;
				Complex<T>  operator+( const Complex<T>& cx ) const;
				Complex<T>  operator-( const Complex<T>& cx ) const;

				Complex<T>& operator*=( const Complex<T>& cx );
				Complex<T>& operator/=( const Complex<T>& cx );
				Complex<T>& operator+=( const Complex<T>& cx );
				Complex<T>& operator-=( const Complex<T>& cx );

				Complex<T>  operator*( T s ) const;
				Complex<T>  operator/( T s ) const;
				Complex<T>  operator+( T s ) const;
				Complex<T>  operator-( T s ) const;

				Complex<T>& operator*=( T s );
				Complex<T>& operator/=( T s );
				Complex<T>& operator+=( T s );
				Complex<T>& operator-=( T s );

				bool operator==( const Complex<T>& cx ) const;
				bool operator!=( const Complex<T>& cx ) const;

				T		   abs( void ) const;
				Complex<T> conj( void ) const;

				T re;
				T im;
		};

		template<typename T>
		inline Complex<T>::Complex()
		{
		}

		template<typename T>
		inline Complex<T>::Complex( T r, T i ) : re( r ), im( i )
		{
		}

		template<typename T>
		inline Complex<T>::Complex(const Complex<T>& cx ) : re( cx.re ), im( cx.im )
		{
		}

		template<typename T>
		inline void Complex<T>::set( T r, T i )
		{
			re = r;
			im = i;
		}

		template<typename T>
		inline void Complex<T>::setZero()
		{
			re = 0;
			im = 0;
		}

		template<typename T>
		inline Complex<T>& Complex<T>::operator=( const Complex<T>& cx )
		{
			re = cx.re;
			im = cx.im;
			return *this;
		}

		template<typename T>
		inline Complex<T> Complex<T>::operator-() const
		{
			return Complex<T>( -re, -im );
		}

		template<typename T>
		inline Complex<T> Complex<T>::operator*( const Complex<T>& cx ) const
		{
			return Complex<T>( re * cx.re - im * cx.im, im * cx.re + re * cx.im );
		}

		template<typename T>
		inline Complex<T> Complex<T>::operator/( const Complex<T>& cx ) const
		{
			T s, t;
			if ( Math::abs( cx.re ) >= Math::abs( cx.im ) ) {
				s = cx.im / cx.re;
				t = ( T ) 1 / ( cx.re + s * cx.im );
				return Complex<T>( ( re + s * im ) * t, ( im - s * re ) * t );
			} else {
				s = cx.re / cx.im;
				t = ( T ) 1 / ( s * cx.re + cx.im );
				return Complex<T>( ( re * s + im ) * t, ( im * s - re ) * t );
			}
		}


		template<typename T>
		inline Complex<T> Complex<T>::operator+( const Complex<T>& cx ) const
		{
			return Complex<T>( re + cx.re, im + cx.im );
		}

		template<typename T>
		inline Complex<T> Complex<T>::operator-( const Complex<T>& cx ) const
		{
			return Complex<T>( re - cx.re, im - cx.im );
		}

		template<typename T>
		inline Complex<T>& Complex<T>::operator*=( const Complex<T>& cx )
		{
			*this = Complex<T>( re * cx.re - im * cx.im, im * cx.re + re * cx.im );
			return *this;
		}

		template<typename T>
		inline Complex<T>& Complex<T>::operator/=( const Complex<T>& cx )
		{
			T s, t;
			if ( Math::abs( cx.re ) >= Math::abs( cx.im ) ) {
				s = cx.im / cx.re;
				t = ( T ) 1 / ( cx.re + s * cx.im );
				*this = Complex<T>( ( re + s * im ) * t, ( im - s * re ) * t );
			} else {
				s = cx.re / cx.im;
				t = ( T ) 1 / ( s * cx.re + cx.im );
				*this = Complex<T>( ( re * s + im ) * t, ( im * s - re ) * t );
			}
			return *this;
		}

		template<typename T>
		inline Complex<T>& Complex<T>::operator+=( const Complex<T>& cx )
		{
			re += cx.re;
			im += cx.im;
			return *this;
		}

		template<typename T>
		inline Complex<T>& Complex<T>::operator-=( const Complex<T>& cx )
		{
			re -= cx.re;
			im -= cx.im;
			return *this;
		}

		template<typename T>
		inline Complex<T> Complex<T>::operator*( T s ) const
		{
			return Complex<T>( re * s , im * s );
		}

		template<typename T>
		inline Complex<T> Complex<T>::operator/( T s ) const
		{
			T inv = ( T ) 1 / s;
			return Complex<T>( re * inv , im * inv );
		}

		template<typename T>
		inline Complex<T> Complex<T>::operator+( T s ) const
		{
			return Complex<T>( re + s , im );
		}

		template<typename T>
		inline Complex<T> Complex<T>::operator-( T s ) const
		{
			return Complex<T>( re - s , im );
		}

		template<typename T>
		inline Complex<T>& Complex<T>::operator*=( T s )
		{
			re *= s;
			im *= s;
			return *this;
		}

		template<typename T>
		inline Complex<T>& Complex<T>::operator/=( T s )
		{
			T inv = ( T ) 1 / s;
			re *= inv;
			im *= inv;
			return *this;
		}

		template<typename T>
		inline Complex<T>& Complex<T>::operator+=( T s )
		{
			re += s;
			return *this;
		}

		template<typename T>
		inline Complex<T>& Complex<T>::operator-=( T s )
		{
			re -= s;
			return *this;
		}

		template<typename T>
		inline Complex<T> operator*( T s, const Complex<T>& cx )
		{
			return Complex<T>( cx.re * s , cx.im * s );
		}

		template<typename T>
		inline Complex<T> operator/( T a, const Complex<T>& cx )
		{
			T s, t;
			if ( Math::abs( cx.re ) >= Math::abs( cx.im ) ) {
				s = cx.im / cx.re;
				t = a / ( cx.re + s * cx.im );
				return Complex<T>( t, - s * t );
			} else {
				s = cx.re / cx.im;
				t = a / ( s * cx.re + cx.im );
				return Complex<T>( s * t, - t );
			}
		}

		template<typename T>
		inline Complex<T> operator+( T s, const Complex<T>& cx )
		{
			return Complex<T>( cx.re + s , cx.im );
		}

		template<typename T>
		inline Complex<T> operator-( T s, const Complex<T>& cx )
		{
			return Complex<T>( s - cx.re, - cx.im );
		}


		template<typename T>
		inline Complex<T> Complex<T>::conj() const
		{
			return Complex<T>( re, -im );
		}

		template<typename T>
		inline T Complex<T>::abs( void ) const
		{
			if( im == 0 )
				return re;
			else if( re == 0 )
				return im;
			else
				return Math::sqrt( Math::sqr( re) + Math::sqr( im ) );
		}

		template<typename T>
		bool Complex<T>::operator==( const Complex<T>& cx ) const
		{
			return ( re == cx.re && im == cx.im );
		}

		template<typename T>
		bool Complex<T>::operator!=( const Complex<T>& cx ) const
        {
			return ( re != cx.re || im != cx.im );
		}

		template<typename T>
		static inline std::ostream& operator<<( std::ostream& out, const Complex<T>& cx )
		{
			out << cx.re << ", " << cx.im;
			return out;
		}

		typedef Complex<float> Complexf;
		typedef Complex<double> Complexd;
}

#endif
