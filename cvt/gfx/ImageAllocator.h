#ifndef IMAGEALLOCATOR_H
#define IMAGEALLOCATOR_H
#include <cvt/gfx/ImageTypes.h>
#include <cvt/util/Rect.h>

namespace cvt {

	enum IAllocatorType {
		IALLOCATOR_MEM = 0
	};

	class ImageAllocator {
		friend class Image;
		friend class ImageAllocatorMem;

		public:
			virtual ~ImageAllocator() {};
			virtual void alloc( size_t width, size_t height, const IOrder order, const IType type ) = 0;
			virtual void copy( const ImageAllocator* x, const Recti* r = NULL ) = 0;
			virtual void retain() = 0;
			virtual void release() = 0;
			virtual uint8_t* map() = 0;
			virtual const uint8_t* map() const = 0;
			virtual void unmap() const = 0;
			virtual IAllocatorType type() = 0;

		protected:
			ImageAllocator() : _width( 0 ), _height( 0 ), _stride( 0 ), _order( IOrder::RGBA ), _type( IType::UBYTE )  {};
			ImageAllocator( const ImageAllocator& );

		protected:
			size_t _width;
			size_t _height;
			size_t _stride;
			IOrder _order;
			IType _type;
	};
}

#endif
