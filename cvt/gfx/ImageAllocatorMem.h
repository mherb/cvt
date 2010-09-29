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
			virtual uint8_t* map( size_t* stride ) { *stride = _stride; return _data; };
			virtual const uint8_t* map( size_t* stride ) const { *stride = _stride; return _data; };
			virtual void unmap( const uint8_t* ptr ) const {};
			virtual IAllocatorType type() const { return IALLOCATOR_MEM; };

		private:
			ImageAllocatorMem( const ImageAllocatorMem& );
			void retain();
			void release();

		private:
			uint8_t* _data;
			size_t _stride;
			uint8_t* _mem;
			size_t* _refcnt;
	};
}

#endif
