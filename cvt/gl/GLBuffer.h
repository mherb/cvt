#ifndef CVTGLBUFFER_H
#define CVTGLBUFFER_H

#include <cvt/gl/OpenGL.h>

namespace cvt {
	class GLBuffer {
		public:
			GLBuffer( GLenum target );
			~GLBuffer();
			void alloc( GLenum usage, size_t size, const void *data = NULL );
			void bind() const;
			size_t size() const;
			GLenum target() const;
			void* map( size_t offset, size_t length, GLbitfield	access );
			void sync( size_t offset, size_t length );
			void unmap();

		private:
			GLuint _buffer;
			GLenum _target;
			size_t _size;
			GLbitfield _access;
	};

	inline GLBuffer::GLBuffer( GLenum target ) : _buffer( 0 ), _target( target ), _size( 0 ), _access( 0 )
	{
		glGenBuffers( 1, &_buffer );
	}

	inline GLBuffer::~GLBuffer()
	{
		glDeleteBuffers( 1, &_buffer );
	}

	inline void GLBuffer::bind() const
	{
		glBindBuffer( _target, _buffer );
	}

	inline size_t GLBuffer::size() const
	{
		return _size;
	}

	inline GLenum GLBuffer::target() const
	{
		return _target;
	}

	inline void GLBuffer::alloc( GLenum usage, size_t size, const void* data )
	{
		glBindBuffer( _target, _buffer );
		_size = size;
		glBufferData( _target, ( GLsizeiptr ) size, data, usage );
		glBindBuffer( _target, 0 );
	}

	inline void* GLBuffer::map( size_t offset, size_t length, GLbitfield access )
	{
		void* ptr;
		glBindBuffer( _target, _buffer );
		_access = ( access & GL_MAP_READ_BIT ) | ( access & GL_MAP_WRITE_BIT );
		ptr = glMapBufferRange( _target, ( GLintptr ) offset, ( GLsizeiptr ) length, _access | GL_MAP_FLUSH_EXPLICIT_BIT  );
		glBindBuffer( _target, 0 );
		return _ptr;
	}

	inline void GLBuffer::sync( size_t offset, size_t length )
	{
		if( ! ( access & GL_MAP_WRITE_BIT ) )
			return;
		glBindBuffer( _target, _buffer );
		glFlushMappedBufferRange( _target, ( GLintptr ) offset, ( GLsizeiptr ) length );
		glBindBuffer( _target, 0 );
	}

	inline void GLBuffer::unmap()
	{
		glBindBuffer( _target, _buffer );
		glUnmapBuffer( _target );
		access = 0;
		glBindBuffer( _target, 0 );
	}

}

#endif
