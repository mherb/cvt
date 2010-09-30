#ifndef CVTIMAGE_H
#define CVTIMAGE_H
#include <iostream>

#include <stdlib.h>
#include <stdint.h>
#include <cv.h>

#include "gfx/Color.h"
#include "gfx/IScaleFilter.h"
#include "gfx/IFilterParameter.h"
#include "util/Rect.h"
#include "gfx/ImageTypes.h"
#include "gfx/ImageAllocator.h"

namespace cvt {

	class Image : public IFilterParameter
	{
		friend std::ostream& operator<<(std::ostream &os, const Image &f);
		friend class CLKernel;

		public:
			Image( size_t w = 1, size_t h = 1, IOrder order = IOrder::RGBA, IType type = IType::UBYTE, IAllocatorType memtype = IALLOCATOR_MEM );
			Image( const Image& img, IAllocatorType memtype = IALLOCATOR_MEM );
			Image( const Image& source, const Recti* roi, bool ref = false, IAllocatorType memtype = IALLOCATOR_MEM );
			~Image();
			size_t width() const;
			size_t height() const;
			size_t channels() const;
			// bits per channel
			size_t bpc() const;
			// bytes per pixel
			size_t bpp() const;
			const IOrder order() const;
			const IType type() const;
			IAllocatorType memType() const { return _mem->type(); };
			uint8_t* map( size_t* stride ) { return _mem->map( stride ); };
			uint8_t const* map( size_t* stride ) const { return ( const uint8_t* ) _mem->map( stride ); };
			void unmap( const uint8_t* ptr ) const { _mem->unmap( ptr ); };
/*			uint8_t* scanline( size_t i );
			uint8_t const* scanline( size_t i ) const;*/

			void reallocate( size_t w, size_t h, IOrder order = IOrder::RGBA, IType type = IType::UBYTE, IAllocatorType memtype = IALLOCATOR_MEM );
			void reallocate( const Image& i, IAllocatorType memtype = IALLOCATOR_MEM );

			void copy( const Image& i );
			void copyRect( int x, int y, const Image& i, int sx, int sy, int swidth, int sheight );

			IplImage* iplimage() const;

			Image* clone() const;
			void convert( Image& dst, IOrder order, IType type ) const;
			void convert( Image& dst ) const;
			void scale( Image& dst, size_t width, size_t height, const IScaleFilter& filter ) const;

			void fill( const Color& c );

			void add( float alpha = 0.0f );
			void sub( float alpha = 0.0f );
			void mul( float alpha = 1.0f );
			void add( const Image& i );
			void sub( const Image& i );
			void mul( const Image& i );
			void mad( const Image& i, float alpha = 1.0f );

			float ssd( const Image& i ) const;
			float sad( const Image& i ) const;

			void add( const Color& c );
			void sub( const Color& c );
			void mul( const Color& c );

			void convolve( Image& dst, const Image& kernel, bool normalize = true ) const;

			Image& operator=( const Color& c );
			Image& operator=( const Image& c );

			Image& operator*( float alpha );
			Image& operator+( float alpha );
			Image& operator-( float alpha );

			Image& operator*( const Color& c );
			Image& operator+( const Color& c );
			Image& operator-( const Color& c );

			Image& operator+( const Image& i );
			Image& operator-( const Image& i );

/*			Color operator() (int x, int y) const;
			Color operator() (float x, float y) const;*/

			void warpBilinear( Image& idst, const Image& warp ) const;
			void debayer( Image& dst, IBayerPattern_t pattern ) const;

		private:
			void upateIpl();
			float* imageToKernel( const Image& k, bool normalize ) const;
			void convolveFloat( Image& dst, const Image& kernel, bool normalize ) const;
			void scaleFloat( Image& idst, size_t width, size_t height, const IScaleFilter& filter ) const;

			void checkFormat( const Image & img, const char* func, size_t lineNum, IOrder order, IType type ) const;
			void checkSize( const Image & img, const char* func, size_t lineNum, size_t w, size_t h ) const;
			void checkFormatAndSize( const Image & img, const char* func, size_t lineNum ) const;

			ImageAllocator* _mem;
			IplImage* _iplimage;

	};

	std::ostream& operator<<(std::ostream &out, const Image &f);

	inline Image* Image::clone() const
	{
		return new Image( *this );
	}

	const inline IOrder Image::order() const
	{
		return _mem->_order;
	}

	const inline IType Image::type() const
	{
		return _mem->_type;
	}

	inline size_t Image::width() const
	{
		return _mem->_width;
	}

	inline size_t Image::height() const
	{
		return _mem->_height;
	}

	/*inline uint8_t* Image::scanline( size_t y )
	{
		y = Math::min( y, _mem->_height - 1 );
		return _data + _mem->_stride * y;
	}

	inline uint8_t const* Image::scanline( size_t y ) const
	{
		y = Math::min( y, _mem->_height - 1 );
		return _data + _mem->_stride * y;
	}*/

	inline void Image::reallocate( const Image& i, IAllocatorType memtype )
	{
		reallocate( i._mem->_width, i._mem->_height, i._mem->_order, i._mem->_type, memtype );
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

	inline Image& Image::operator+( const Image& i )
	{
		add( i );
		return *this;
	}

	inline Image& Image::operator-( const Image& i )
	{
		sub( i );
		return *this;
	}

	inline size_t Image::channels() const
	{
		return _mem->_order.channels;
	}

	inline size_t Image::bpc() const
	{
		return _mem->_type.size * 8;
	}

	inline size_t Image::bpp() const
	{
		return _mem->_type.size * _mem->_order.channels;
	}
}

#endif
