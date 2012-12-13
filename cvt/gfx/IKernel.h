/*
			CVT - Computer Vision Tools Library

 	 Copyright (c) 2012, Philipp Heise, Sebastian Klose

 	THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 	KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 	IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 	PARTICULAR PURPOSE.
 */
#ifndef CVT_IKERNEL_H
#define CVT_IKERNEL_H

#include <cvt/util/SIMD.h>

namespace cvt {
	class IKernel {
		public:
			IKernel();
			IKernel( const IKernel& kernel );
			IKernel( size_t w, size_t h, float* data );
			IKernel( size_t w, size_t h );
			~IKernel();
			IKernel& operator=(const IKernel& kernel );

			size_t width() const;
			size_t height() const;

			float& operator()( int x, int y );
			const float& operator()( int x, int y ) const;
			const float* ptr() const;

			float sum() const;
			void normalize();
			void scale( float value );

			bool isSymmetrical() const;
			bool isPointSymmetrical() const;

			static const IKernel GAUSS_HORIZONTAL_3;
			static const IKernel GAUSS_HORIZONTAL_5;
			static const IKernel GAUSS_HORIZONTAL_7;

			static const IKernel MEAN_HORIZONTAL_3;
			static const IKernel MEAN_HORIZONTAL_5;
			static const IKernel MEAN_HORIZONTAL_7;

			static const IKernel HAAR_HORIZONTAL_2;
			static const IKernel HAAR_HORIZONTAL_3;

			static const IKernel GAUSS_VERTICAL_3;
			static const IKernel GAUSS_VERTICAL_5;
			static const IKernel GAUSS_VERTICAL_7;

			static const IKernel MEAN_VERTICAL_3;
			static const IKernel MEAN_VERTICAL_5;
			static const IKernel MEAN_VERTICAL_7;

			static const IKernel HAAR_VERTICAL_2;
			static const IKernel HAAR_VERTICAL_3;
			static const IKernel FIVEPOINT_DERIVATIVE_HORIZONTAL;
			static const IKernel FIVEPOINT_DERIVATIVE_VERTICAL;

			static const IKernel LAPLACE_33;

			static const IKernel LAPLACE_3_XX;
			static const IKernel LAPLACE_3_YY;

			static const IKernel LAPLACE_5_XX;
			static const IKernel LAPLACE_5_YY;

		private:
			size_t _width, _height;
			float* _mem;
			float* _data;
	};

	inline IKernel::IKernel() : _width( 0 ), _height( 0 ), _mem( NULL ), _data( NULL )
	{
	}

	inline IKernel::IKernel( const IKernel& kernel ) : _width( kernel._width ),
													   _height( kernel._height )
	{
		if( &kernel == this )
			return;
		_mem = new float[ _width * _height ];
		_data = _mem;
		SIMD* simd = SIMD::instance();
		simd->Memcpy( ( uint8_t* ) _data, ( const uint8_t* ) kernel._data, _width * _height * sizeof( float ) );
	}


	inline IKernel::IKernel( size_t w, size_t h ) : _width( w ),
												    _height( h )
	{
		_mem = new float[ _width * _height ];
		_data = _mem;
	}

	inline IKernel& IKernel::operator=(const IKernel& kernel )
	{
		if( &kernel == this )
			return *this;
		_width = kernel._width;
		_height = kernel._height;
		if( _mem )
			delete[] _mem;
		_mem = new float[ _width * _height ];
		_data = _mem;
		SIMD* simd = SIMD::instance();
		simd->Memcpy( ( uint8_t* ) _data, ( const uint8_t* ) kernel._data, _width * _height * sizeof( float ) );
		return *this;
	}

	inline IKernel::IKernel( size_t w, size_t h, float* data ) : _width( w ),
																	   _height( h ),
																	   _mem( NULL ),
																	   _data( data )
	{
	}

	inline IKernel::~IKernel()
	{
		if( _mem )
			delete[] _mem;
	}

	inline size_t IKernel::width() const
	{
		return _width;
	}

	inline size_t IKernel::height() const
	{
		return _height;
	}

	inline float& IKernel::operator()( int x, int y )
	{
		return _data[ y * _width + x ];
	}

	inline const float& IKernel::operator()( int x, int y ) const
	{
		return _data[ y * _width + x ];
	}

	inline const float* IKernel::ptr() const
	{
		return _data;
	}

	inline float IKernel::sum() const
	{
		size_t n = _width * _height;
		float* ptr = _data;
		float ret = 0;
		while( n-- )
			ret += *ptr++;
		return ret;
	}

	inline void IKernel::normalize()
	{
		size_t n = _width * _height;
		float* ptr = _data;
		float invsum = 1.0f / sum();
		while( n-- )
			*ptr++ *= invsum;
	}

	inline void IKernel::scale ( float value )
	{
		size_t n = _width * _height;
		float* ptr = _data;
		while( n-- )
			*ptr++ *= value;
	}


	inline bool IKernel::isSymmetrical() const
	{
		if( !( _width & 1 ) || !( _height & 1 ) )
			return false;

		const size_t iend = _width * _height;
		for( size_t i = 0; i < iend; i++ )
		{
			float a = _data[ i ];
			float b = _data[ iend - i - 1 ];
			if( a != b )
				return false;
		}
		return true;
	}

	inline bool IKernel::isPointSymmetrical() const
	{
		if( !( _width & 1 ) || !( _height & 1 ) )
			return false;

		const size_t iend = _width * _height;
		for( size_t i = 0; i < iend; i++ )
		{
			if( i == iend - i - 1 )
				continue;
			float a = _data[ i ];
			float b = _data[ iend - i - 1 ];
			if( a != -b )
				return false;
		}
		return true;
	}

}

#endif
