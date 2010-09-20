#ifndef IMAGEALLOCATORMEM_H
#define IMAGEALLOCATORMEM_H
#include <cvt/gfx/ImageAllocator.h>

namespace cvt {
	class ImageAllocatorMem : public ImageAllocator {
		public:
			ImageAllocatorMem();
			~ImageAllocatorMem();
			virtual void alloc( size_t width, size_t height, const IOrder order, const IType type );
			virtual void copy( const ImageAllocator* x, const Recti* r );
			virtual void retain();
			virtual void release();
			virtual uint8_t* map() { return _data; };
			virtual const uint8_t* map() const { return _data; };
			virtual void unmap() const {};
			virtual IAllocatorType type() { return IALLOCATOR_MEM; };

		private:
			ImageAllocatorMem( const ImageAllocatorMem& );

		private:
			uint8_t* _data;
			uint8_t* _mem;
			size_t* _refcnt;
	};
}

#endif
