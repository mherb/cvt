#ifndef IMAGEALLOCATOR_H
#define IMAGEALLOCATOR_H

#include <cvt/gfx/IFormat.h>
#include <cvt/geom/Rect.h>

namespace cvt {

	enum IAllocatorType {
		IALLOCATOR_MEM = ( 0 ),
		IALLOCATOR_CL = ( 1 << 0 ),
		IALLOCATOR_GL = ( 1 << 1 )
	};

	class ImageAllocator {
		friend class Image;
		friend class ImageAllocatorMem;
		friend class ImageAllocatorCL;
		friend class ImageAllocatorGL;

		public:
			virtual ~ImageAllocator() {};
			virtual void alloc( size_t width, size_t height, const IFormat & format ) = 0;
			virtual void copy( const ImageAllocator* x, const Recti* r = NULL ) = 0;
			virtual uint8_t* map( size_t* stride ) = 0;
			virtual const uint8_t* map( size_t* stride ) const = 0;
			virtual void unmap( const uint8_t* ptr ) const = 0;
			virtual IAllocatorType type() const = 0;

		protected:
			ImageAllocator() : _width( 0 ), _height( 0 ), _format( IFormat::RGBA_UINT8 ) {};
			ImageAllocator( const ImageAllocator& );

		protected:
			size_t _width;
			size_t _height;
			IFormat _format;
	};
}

#endif
