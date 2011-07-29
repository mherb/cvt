#ifndef CVTGLBUFFER_H
#define CVTGLBUFFER_H

#include <cvt/gl/OpenGL.h>

namespace cvt {
	class GLBuffer {
		public:
			GLBuffer( GLenum target = GL_ARRAY_BUFFER );
			~GLBuffer();
			void alloc( GLenum usage, size_t size, const void *data = NULL );
			void bind() const;
			void unbind() const;
			size_t size() const;
			GLenum target() const;
			void setData( size_t size, const void* data, size_t offset = 0 );

			void* map( GLenum access = GL_READ_WRITE ) const;
			void unmap() const;
			/*
			void sync( size_t offset, size_t length ) const;
			 */

		private:
			GLuint _buffer;
			GLenum _target;
			size_t _size;
			mutable GLbitfield _access;
	};



	inline size_t GLBuffer::size() const
	{
		return _size;
	}

	inline GLenum GLBuffer::target() const
	{
		return _target;
	}

}

#endif
