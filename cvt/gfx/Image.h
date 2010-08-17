#ifndef CVTIMAGE_H
#define CVTIMAGE_H
#include <iostream>

#include <stdlib.h>
#include <stdint.h>
#include <cv.h>

#include "gfx/Color.h"

namespace cvt {

    enum ImageChannelOrder {
	CVT_GRAY = 0,
	CVT_GRAYALPHA,
	CVT_RGBA,
	CVT_BGRA
    };

    enum ImageChannelType {
	CVT_UBYTE = 0,
	CVT_FLOAT,
    };

    class Image
    {
	friend std::ostream& operator<<(std::ostream &os, const Image &f);

	public:
	    Image( size_t w = 1, size_t h = 1, ImageChannelOrder order = CVT_RGBA, ImageChannelType type = CVT_UBYTE );
	    Image( const Image& img );
	    ~Image();

	    size_t width() const;
	    size_t height() const;
	    size_t stride() const;
	    size_t channels() const;
	    ImageChannelOrder order() const;
	    ImageChannelType type() const;
	    uint8_t* data();
	    void reallocate( size_t w, size_t h, ImageChannelOrder order = CVT_RGBA, ImageChannelType type = CVT_UBYTE );
	    void reallocate( const Image& i );
	    void copy( const Image& i );
	    IplImage* iplimage() const;

	    void convert( Image& dst, ImageChannelOrder order, ImageChannelType type ) const;

	    void fill( const Color& c );

	    void add( float alpha = 0.0f );
	    void sub( float alpha = 0.0f );
	    void mul( float alpha = 1.0f );
	    void mad( const Image& i, float alpha = 1.0f );

	    void add( const Color& c );
	    void sub( const Color& c );
	    void mul( const Color& c );

	    void convolve( Image& dst, const Image& kernel, bool normalize = true );

	    Image& operator=( const Color& c );
	    Image& operator=( const Image& c );

	    Image& operator*( float alpha );
	    Image& operator+( float alpha );
	    Image& operator-( float alpha );

	    Image& operator*( const Color& c );
	    Image& operator+( const Color& c );
	    Image& operator-( const Color& c );



	    /* FIXME: remove - use convolve instead */
	    void ddx( Image& dx, bool forward = true ) const;
	    void ddy( Image& dy, bool forward = true ) const;

	private:
	    void upateIpl();
	    float* imageToKernel( const Image& k, bool normalize );
	    void convolveFloat( Image& dst, const Image& kernel, bool normalize );

	    ImageChannelOrder _order;
	    ImageChannelType _type;
	    size_t _width;
	    size_t _height;
	    size_t _stride;
	    uint8_t* _data;
	    IplImage* _iplimage;


	    static size_t _type_size[];
	    static size_t _order_channels[];
    };

    std::ostream& operator<<(std::ostream &out, const Image &f);

    inline ImageChannelOrder Image::order() const
    {
	return _order;
    }

    inline ImageChannelType Image::type() const
    {
	return _type;
    }

    inline size_t Image::width() const
    {
	return _width;
    }

    inline size_t Image::height() const
    {
	return _height;
    }

    inline size_t Image::stride() const
    {
	return _stride;
    }

    inline uint8_t* Image::data()
    {
	return _data;
    }

    inline void Image::reallocate( const Image& i )
    {
	reallocate( i._width, i._height, i._order, i._type );
    }

    inline IplImage* Image::iplimage() const
    {
	return _iplimage;
    }

    inline Image& Image::operator=( const Image& c )
    {
	copy( c );
	return *this;
    }

    inline Image& Image::operator*( float alpha )
    {
	mul( alpha );
	return *this;
    }

    inline Image& Image::operator+( float alpha )
    {
	add( alpha );
	return *this;
    }

    inline Image& Image::operator-( float alpha )
    {
	sub( alpha );
	return *this;
    }

    inline Image& Image::operator*( const Color& c )
    {
	mul( c );
	return *this;
    }

    inline Image& Image::operator+( const Color& c )
    {
	add( c );
	return *this;
    }
    inline Image& Image::operator-( const Color& c )
    {
	sub( c );
	return *this;
    }
}

#endif
