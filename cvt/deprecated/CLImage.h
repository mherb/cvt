/*
			CVT - Computer Vision Tools Library

 	 Copyright (c) 2012, Philipp Heise, Sebastian Klose

 	THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 	KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 	IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 	PARTICULAR PURPOSE.
 */
#ifndef CLIMAGE_H
#define CLIMAGE_H

#include <cvt/cl/CLContext.h>
#include <cvt/gfx/Image.h>

namespace cvt {

	class CLImage : public IFilterParameter {
		friend class CLKernel;
		public:
			CLImage( size_t w = 0, size_t h = 0, IOrder order = IOrder::RGBA, IType type = IType::UBYTE );
			CLImage( const CLImage& i );
			CLImage( const Image& i );
			~CLImage();

			size_t width() const { return _width; };
			size_t height() const { return _height; };
			const IOrder order() const { return _order; };
			const IType type() const { return _type; };
			::cl::NDRange globalRange() const { return ::cl::NDRange( _width, _height ); };

			void reallocate( const CLImage& i );
			void copy( const CLImage& i );
			void readImage( Image& i );
			void readData( void* data, size_t stride );
			void writeImage( const Image& i ) const;
			void writeData( void const* data, size_t stride ) const;


		private:
			static ::cl::ImageFormat getCLFormat( IOrder order, IType type );

			CLContext* _cl;
			IOrder _order;
			IType _type;
			size_t _width;
			size_t _height;
			cl::Image2D _climage;
	};

}

#endif
