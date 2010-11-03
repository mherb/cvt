#include <cvt/gl/GLBuffer.h>

namespace cvt {

	GLBuffer::GLBuffer( GLenum target ) : _buffer( 0 ), _target( target ), _size( 0 ), _access( 0 )
	{
		glGenBuffers( 1, &_buffer );
	}

	GLBuffer::~GLBuffer()
	{
		glDeleteBuffers( 1, &_buffer );
	}

	void GLBuffer::bind() const
	{
		glBindBuffer( _target, _buffer );
	}

	void GLBuffer::unbind() const
	{
		glBindBuffer( _target, 0 );
	}

	void GLBuffer::alloc( GLenum usage, size_t size, const void* data )
	{
		glBindBuffer( _target, _buffer );
		_size = size;
		glBufferData( _target, ( GLsizeiptr ) size, data, usage );
		glBindBuffer( _target, 0 );
	}

	void* GLBuffer::map( size_t offset, size_t length, GLbitfield access ) const
	{
		void* ptr;

		if( access )
			return NULL;

		glBindBuffer( _target, _buffer );
		_access = ( access & GL_MAP_READ_BIT ) | ( access & GL_MAP_WRITE_BIT );
		ptr = glMapBufferRange( _target, ( GLintptr ) offset, ( GLsizeiptr ) length, _access | GL_MAP_FLUSH_EXPLICIT_BIT  );
		glBindBuffer( _target, 0 );
		return ptr;
	}

	void GLBuffer::sync( size_t offset, size_t length ) const
	{
		if( ! ( _access & GL_MAP_WRITE_BIT ) )
			return;
		glBindBuffer( _target, _buffer );
		glFlushMappedBufferRange( _target, ( GLintptr ) offset, ( GLsizeiptr ) length );
		glBindBuffer( _target, 0 );
	}

	void GLBuffer::unmap() const
	{
		glBindBuffer( _target, _buffer );
		glUnmapBuffer( _target );
		_access = 0;
		glBindBuffer( _target, 0 );
	}
}
