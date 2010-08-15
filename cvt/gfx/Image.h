#ifndef CVTIMAGE_H
#define CVTIMAGE_H
#include <stdlib.h>
#include <stdint.h>

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
	public:
	    Image( size_t w, size_t h, ImageChannelOrder order = CVT_RGBA, ImageChannelType type = CVT_UBYTE );
	    ~Image();

	    size_t width() const;
	    size_t height() const;
	    size_t stride() const;
	    ImageChannelOrder order() const;
	    ImageChannelType type() const;
	    uint8_t* data();
	    void reallocate( size_t w, size_t h, ImageChannelOrder order = CVT_RGBA, ImageChannelType type = CVT_UBYTE );

	private:
	    ImageChannelOrder _order;
	    ImageChannelType _type;
	    size_t _width;
	    size_t _height;
	    size_t _stride;
	    uint8_t* _data;
    };

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

}

#endif
