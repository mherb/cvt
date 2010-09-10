#ifndef CLIMAGE_H
#define CLIMAGE_H

#include <cvt/cl/CLContext.h>
#include <cvt/gfx/Image.h>

namespace cvt {

	class CLImage : public IFilterParameter {
		friend class CLKernel;
		public:
			CLImage( size_t w = 0, size_t h = 0, ImageChannelOrder order = CVT_RGBA, ImageChannelType type = CVT_UBYTE );
			CLImage( const CLImage& i );
			CLImage( const Image& i );
			~CLImage();

			size_t width() const { return _width; };
			size_t height() const { return _height; };
			ImageChannelOrder order() const { return _order; };
			ImageChannelType type() const { return _type; };
			::cl::NDRange globalRange() const { return ::cl::NDRange( _width, _height ); };

			void reallocate( const CLImage& i );
			void copy( const CLImage& i );
			void readImage( Image& i );
			void readData( void* data, size_t stride );
			void writeImage( const Image& i ) const;
			void writeData( void const* data, size_t stride ) const;


		private:
			static ::cl::ImageFormat getCLFormat( ImageChannelOrder order, ImageChannelType type );

			CLContext* _cl;
			ImageChannelOrder _order;
			ImageChannelType _type;
			size_t _width, _height;
			cl::Image2D _climage;
	};

}

#endif
