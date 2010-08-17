#ifndef CVTCOLOR_H
#define CVTCOLOR_H

#include "math/Math.h"

namespace cvt {
    class Image;

    class Color
    {
	friend class Image;

	public:
	    Color( float r, float g, float b, float a );
	    Color( float gray );
	    Color( int r, int g, int b, int a );

	    void set( float r, float g, float b, float a );
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

	private:
	    float _r, _g, _b, _a;
    };


    inline Color::Color( float r, float g, float b, float a )
    {
	set( r, g, b, a );
    }

    inline Color::Color( float g )
    {
	set( g, g, g, 1.0f );
    }

    inline Color::Color( int r, int g, int b, int a )
    {
	_r = Math::clamp( ( float ) r / 255.0f, 0.0f, 1.0f );
	_g = Math::clamp( ( float ) g / 255.0f, 0.0f, 1.0f );
	_b = Math::clamp( ( float ) b / 255.0f, 0.0f, 1.0f );
	_a = Math::clamp( ( float ) a / 255.0f, 0.0f, 1.0f );
    }

    inline void Color::set( float r, float g, float b, float a )
    {
	_r = Math::clamp( r, 0.0f, 1.0f );
	_g = Math::clamp( g, 0.0f, 1.0f );
	_b = Math::clamp( b, 0.0f, 1.0f );
	_a = Math::clamp( a, 0.0f, 1.0f );
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

}

#endif
