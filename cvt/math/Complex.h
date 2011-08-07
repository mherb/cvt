#ifndef CVT_COMPLEX_H
#define CVT_COMPLEX_H

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

				Complex<T> sqrt( void ) const;
				Complex<T> abs( void ) const;

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
		inline Complex<T>::set( T r, T i )
		{
			re = r;
			im = i;
		}


		template<typename T>
		inline Complex<T>::setZero()
		{
			re = 0;
			im = 0;
		}

		template<typename T>
		inline Complex<T>& Complex<T>::operator=( const Complex<T>& cx )
		{
			if( cx == *this ) return *this;
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
		inline Complex<T> Complex<T>::operator+( const Complex<T>& cx ) const
		{
			return Complex<T>( re + cx.re, im + cx.im );
		}

		template<typename T>
		inline Complex<T> Complex<T>::operator-( const Complex<T>& cx ) const
		{
			return Complex<T>( re - cx.re, im - cx.im );
		}


}

#endif
