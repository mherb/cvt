#ifndef IMAGEALLOCATORGL_H
#define IMAGEALLOCATORGL_H
#include <cvt/gfx/ImageAllocator.h>
#include <cvt/gl/OpenGL.h>

namespace cvt {
	class ImageAllocatorGL : public ImageAllocator {
		friend class GLDrawImageProg;
		friend class GLTexMLSProg;
		public:
			ImageAllocatorGL();
			~ImageAllocatorGL();
			virtual void alloc( size_t width, size_t height, const IFormat & format );
			virtual void copy( const ImageAllocator* x, const Recti* r );
			virtual uint8_t* map( size_t* stride );
			virtual const uint8_t* map( size_t* stride ) const;
			virtual void unmap( const uint8_t* ptr ) const;
			virtual IAllocatorType type() const { return IALLOCATOR_GL; };

		private:
			ImageAllocatorGL( const ImageAllocatorGL& );
			void getGLFormat( const IFormat & format, GLenum& glformat, GLenum& gltype ) const;

		private:
			GLuint _tex2d;
			GLuint _glbuf;
			size_t _stride;
			size_t _size;
			/* PBO can only be mapped once, we need refcounting in our const methods and
			   need to check if the buffer is dirty ( only in case of non-const map )  */
			mutable void* _ptr;
			mutable int _ptrcount;
			mutable bool _dirty;
	};
}

#endif
