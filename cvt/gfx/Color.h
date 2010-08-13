#ifndef CVTCOLOR_H
#define CVTCOLOR_H

#include "math/Math.h"

namespace cvt {

    class Color
    {
	public:
	    Color( float r, float g, float b, float a )
	    Color( uint8_t r, uint8_t g, uint8_t b, uint8_t a );

	    set( float r, float g, float b, float a );
	    set( uint8_t r, uint8_t g, uint8_t b, uint8_t a );

	    get( float& r, float& g, float& b, float& a ) const;
	    get( uint8_t& r, uint8_t& g, uint8_t& b, uint8_t& a ) const;

	    float red() const { return _r; };
	    float green() const { return _g; };
	    float blue() const { return _b; };
	    float alpha() const { return _a; };

	    void setRed( float r ) { _r = r; };
	    void setGreen( float g ) { _g = g; };
	    void setBlue( float b ) { _b = b; };
	    void setAlpha( float a ) { _a = a; };


	private:
	    float _r, _g, _b, _a;
    };

    Color::Color( float r, float g, float b, float a )
    {
	set( r, g, b, a );
    };

    Color::Color( uint8_t r, uint8_t g, uint8_t b, uint8_t a )
    {
	_r = ( float ) r / 255.0f;
	_g = ( float ) g / 255.0f;
	_b = ( float ) b / 255.0f;
	_a = ( float ) a / 255.0f;
    };

    Color::set( float r, float g, float b, float a )
    {
	_r = Math::clamp( r, 0.0f, 1.0f );
	_g = Math::clamp( g, 0.0f, 1.0f );
	_b = Math::clamp( b, 0.0f, 1.0f );
	_a = Math::clamp( a, 0.0f, 1.0f );
    };

    Color::set( uint8_t r, uint8_t g, uint8_t b, uint8_t a )
    {
	_r = ( float ) r / 255.0f;
	_g = ( float ) g / 255.0f;
	_b = ( float ) b / 255.0f;
	_a = ( float ) a / 255.0f;
    }

    inline void Color::get( float& r, float& g, float& b, float& a ) const
    {
	r = _r;
	g = _g;
	b = _b;
	a = _a;
    };

    inline void Color::get( uint8_t& r, uint8_t& g, uint8_t& b, uint8_t& a ) const
    {
	r = ( uint8_t ) ( _r * 255.0f );
	g = ( uint8_t ) ( _g * 255.0f );
	b = ( uint8_t ) ( _b * 255.0f );
	a = ( uint8_t ) ( _a * 255.0f );
    };

}

#endif
