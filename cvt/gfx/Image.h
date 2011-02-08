#ifndef CVTIMAGE_H
#define CVTIMAGE_H
#include <iostream>

#include <stdlib.h>
#include <stdint.h>

#include <cvt/gfx/Color.h>
#include <cvt/gfx/IFormat.h>
#include <cvt/gfx/IScaleFilter.h>
#include <cvt/geom/Rect.h>
#include <cvt/gfx/ImageAllocator.h>
#include <cvt/gfx/IKernel.h>

namespace cvt {

	class Image
	{
		friend std::ostream& operator<<(std::ostream &os, const Image &f);
		friend class CLKernel;
		friend class GLDrawImageProg;

		public:
			Image( size_t w = 1, size_t h = 1, const IFormat & format = IFormat::RGBA_UINT8, IAllocatorType memtype = IALLOCATOR_MEM );
			Image( size_t w, size_t h, const IFormat & format, uint8_t* data, size_t stride = 0 );
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
			const IFormat & format() const;
			IAllocatorType memType() const { return _mem->type(); };
			uint8_t* map( size_t* stride ) { return _mem->map( stride ); };
			const uint8_t * map( size_t* stride ) const { return ( const uint8_t* ) _mem->map( stride ); };
			template<typename _T> _T* map( size_t* stride );
			template<typename _T> const _T* map( size_t* stride ) const;
			void unmap( const uint8_t* ptr ) const { _mem->unmap( ptr ); };
			template<typename _T> void unmap( const _T* ptr ) const;

			void reallocate( size_t w, size_t h, const IFormat & format = IFormat::RGBA_UINT8, IAllocatorType memtype = IALLOCATOR_MEM );
			void reallocate( const Image& i, IAllocatorType memtype = IALLOCATOR_MEM );

			void copy( const Image& i );
			void copyRect( int x, int y, const Image& i, int sx, int sy, int swidth, int sheight );

			Image* clone() const;
			void convert( Image& dst, const IFormat & format ) const;
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

			void convolve( Image& dst, const IKernel& kernel ) const;
//			void convolve( Image& dst, const IKernel& hkernel, const IKernel& vkernel ) const;

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
			
		private:
			void convolveFloat( Image& dst, const IKernel& kernel ) const;
			void convolveU8( Image& idst, const IKernel& kernel ) const;
			void scaleFloat( Image& idst, size_t width, size_t height, const IScaleFilter& filter ) const;
			void scaleFixedu8( Image& idst, size_t width, size_t height, const IScaleFilter& filter ) const;

			void checkFormat( const Image & img, const char* func, size_t lineNum, const IFormat & format ) const;
			void checkSize( const Image & img, const char* func, size_t lineNum, size_t w, size_t h ) const;
			void checkFormatAndSize( const Image & img, const char* func, size_t lineNum ) const;

			ImageAllocator* _mem;
	};

	std::ostream& operator<<(std::ostream &out, const Image &f);

	inline Image* Image::clone() const
	{
		return new Image( *this );
	}

	const inline IFormat & Image::format() const
	{
		return _mem->_format;
	}

	inline size_t Image::width() const
	{
		return _mem->_width;
	}

	inline size_t Image::height() const
	{
		return _mem->_height;
	}

	inline void Image::reallocate( const Image& i, IAllocatorType memtype )
	{
		reallocate( i._mem->_width, i._mem->_height, i._mem->_format, memtype );
	}

	template<typename _T>
	inline _T* Image::map( size_t* stride )
	{
		uint8_t* ret = _mem->map( stride );
		*stride /= sizeof( _T );
		return ( _T * ) ret;
	}

	template<typename _T>
	inline const _T* Image::map( size_t* stride ) const
	{
		const uint8_t* ret = _mem->map( stride );
		*stride /= sizeof( _T );
		return ( const _T * ) ret;
	}

	template<typename _T>
	inline void Image::unmap( const _T* ptr ) const
	{
		 _mem->unmap( ( uint8_t* ) ptr );
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
		return _mem->_format.channels;
	}

	inline size_t Image::bpc() const
	{
		return _mem->_format.bpc;
	}

	inline size_t Image::bpp() const
	{
		return _mem->_format.bpp;
	}
}

#endif
