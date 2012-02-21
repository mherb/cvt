/*
			CVT - Computer Vision Tools Library

 	 Copyright (c) 2012, Philipp Heise, Sebastian Klose

 	THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 	KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 	IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 	PARTICULAR PURPOSE.
 */
#ifndef IMAGEALLOCATORMEM_H
#define IMAGEALLOCATORMEM_H
#include <cvt/gfx/ImageAllocator.h>

namespace cvt {
	class ImageAllocatorMem : public ImageAllocator {
		public:
			ImageAllocatorMem();
			~ImageAllocatorMem();
			virtual void alloc( size_t width, size_t height, const IFormat & format );
			void alloc( size_t width, size_t height, const IFormat & format, uint8_t* data, size_t stride = 0 );
			virtual void copy( const ImageAllocator* x, const Recti* r );
			virtual uint8_t* map( size_t* stride ) { *stride = _stride; return _data; };
			virtual const uint8_t* map( size_t* stride ) const { *stride = _stride; return _data; };
			virtual void unmap( const uint8_t* ) const {};
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
