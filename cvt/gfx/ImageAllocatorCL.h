#ifndef IMAGEALLOCATORCL_H
#define IMAGEALLOCATORCL_H
#include <cvt/gfx/ImageAllocator.h>
#include <cvt/cl/CLImage2D.h>

namespace cvt {
	class ImageAllocatorCL : public ImageAllocator {
		friend class CLKernel;

		public:
			ImageAllocatorCL();
			~ImageAllocatorCL();
			virtual void alloc( size_t width, size_t height, const IFormat & format );
			virtual void copy( const ImageAllocator* x, const Recti* r );
			virtual uint8_t* map( size_t* stride );
			virtual const uint8_t* map( size_t* stride ) const;
			virtual void unmap( const uint8_t* ptr ) const;
			virtual IAllocatorType type() const { return IALLOCATOR_CL; };

		private:
			ImageAllocatorCL( const ImageAllocatorCL& );

		private:
			CLImage2D* _climage;
	};
}

#endif
