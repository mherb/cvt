#ifndef CVTCOLOR_H
#define CVTCOLOR_H

#include <cvt/math/Math.h>
#include <cvt/gfx/ImageTypes.h>
#include <cvt/gfx/IFilterParameter.h>

namespace cvt {
	class Image;

	class Color : public IFilterParameter
	{
		friend class Image;
		friend class CLKernel;

		public:
			Color();
			Color( float r, float g, float b, float a );
			Color( float gray );
			Color( float gray, float alpha );
			Color( int r, int g, int b, int a );
			Color( int gray );
			Color( int gray, int alpha );

			void set( float r, float g, float b, float a );
			void set( float g, float a );
			void set( int r, int g, int b, int a );

			void get( float& r, float& g, float& b, float& a ) const;
			void get( int& r, int& g, int& b, int& a ) const;

			float red() const { return _r; };
			float green() const { return _g; };
			float blue() const { return _b; };
			float alpha() const { return _a; };
			float gray() const;

			void setRed( float r ) { _r = r; };
			void setGreen( float g ) { _g = g; };
			void setBlue( float b ) { _b = b; };
			void setAlpha( float a ) { _a = a; };

			Color& operator=( const Color & c );
			Color operator+( const Color & c ) const;
			Color operator-( const Color & c ) const;
			Color operator*( float s ) const;

			const float* data() const { return v; };

		private:
			union {
				struct {
					float _r, _g, _b, _a;
				};
				float v[ 4 ];
			}  __attribute__((aligned(16)));
	};

	inline Color::Color() : IFilterParameter( IFILTERPARAMETER_COLOR )
	{
		set( 0.0f, 0.0f, 0.0f, 0.0f );
	}

	inline Color::Color( float r, float g, float b, float a ) : IFilterParameter( IFILTERPARAMETER_COLOR )
	{
		set( r, g, b, a );
	}

	inline Color::Color( float g ) : IFilterParameter( IFILTERPARAMETER_COLOR )
	{
		set( g, g, g, 1.0f );
	}

	inline Color::Color( float g, float alpha ) : IFilterParameter( IFILTERPARAMETER_COLOR )
	{
		set( g, g, g, alpha );
	}

	inline Color::Color( int r, int g, int b, int a ) : IFilterParameter( IFILTERPARAMETER_COLOR )
	{
		_r = Math::clamp( ( float ) r / 255.0f, 0.0f, 1.0f );
		_g = Math::clamp( ( float ) g / 255.0f, 0.0f, 1.0f );
		_b = Math::clamp( ( float ) b / 255.0f, 0.0f, 1.0f );
		_a = Math::clamp( ( float ) a / 255.0f, 0.0f, 1.0f );
	}
	
	inline Color::Color( int gray, int alpha ) : IFilterParameter( IFILTERPARAMETER_COLOR )
	{
		_r = Math::clamp( ( float ) gray / 255.0f, 0.0f, 1.0f );
		_g = _r;
		_b = _r;
		_a = Math::clamp( ( float ) alpha / 255.0f, 0.0f, 1.0f );
	}
	
	inline Color::Color( int gray ) : IFilterParameter( IFILTERPARAMETER_COLOR )
	{
		_r = Math::clamp( ( float ) gray / 255.0f, 0.0f, 1.0f );
		_g = _r;
		_b = _r;
		_a = 1.0f;
	}

	inline void Color::set( float r, float g, float b, float a )
	{
		_r = r;
		_g = g;
		_b = b;
		_a = a;
	}

	inline void Color::set( float g, float a )
	{
		_r = g;
		_g = g;
		_b = g;
		_a = a;
	}

	inline void Color::set( int r, int g, int b, int a )
	{
		_r = Math::clamp( ( float ) r / 255.0f, 0.0f, 1.0f );
		_g = Math::clamp( ( float ) g / 255.0f, 0.0f, 1.0f );
		_b = Math::clamp( ( float ) b / 255.0f, 0.0f, 1.0f );
		_a = Math::clamp( ( float ) a / 255.0f, 0.0f, 1.0f );
	}

	inline void Color::get( float& r, float& g, float& b, float& a ) const
	{
		r = _r;
		g = _g;
		b = _b;
		a = _a;
	}

	inline void Color::get( int& r, int& g, int& b, int& a ) const
	{
		r = ( int ) ( _r * 255.0f );
		g = ( int ) ( _g * 255.0f );
		b = ( int ) ( _b * 255.0f );
		a = ( int ) ( _a * 255.0f );
	}

	inline float Color::gray() const
	{
		return 0.2126f * _r + 0.7152f * _g + 0.0722f * _b;
	}

	inline Color& Color::operator=( const Color & c )
	{
		_r = c._r;
		_g = c._g;
		_b = c._b;
		_a = c._a;
		return *this;
	}

	inline Color Color::operator+( const Color & c ) const
	{
		Color r;
		r._r = _r + c._r;
		r._g = _g + c._g;
		r._b = _b + c._b;
		r._a = _a + c._a;
		
		return r;
	}
	
	inline Color Color::operator-( const Color & c ) const
	{
		Color r;
		r._r = _r - c._r;
		r._g = _g - c._g;
		r._b = _b - c._b;
		r._a = _a - c._a;
	
		return r;
	}
	
	inline Color Color::operator*( float s ) const
	{
		Color r;
		r._r = _r * s;
		r._g = _g * s;
		r._b = _b * s;
		r._a = _a * s;
		return r;
	}

}

#endif
