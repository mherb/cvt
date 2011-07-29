#include <cvt/gl/GLBuffer.h>
#include <cvt/util/SIMD.h>

namespace cvt {

#ifdef __APPLE__
#define glFlushMappedBufferRange glFlushMappedBufferRangeAPPLE
#endif

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


	void GLBuffer::setData( size_t size, const void* data, size_t offset )
	{
		glBindBuffer( _target, _buffer );
		if( ( offset + size ) > _size )
			throw CVTException( "Provided size exceeds buffer size!" );
		glBufferSubData( _target, offset, size, data );
		glBindBuffer( _target, 0 );
	}

	void* GLBuffer::map( GLenum access ) const
	{
		void* ptr;

		glBindBuffer( _target, _buffer );
		ptr = glMapBuffer( _target, access );
		glBindBuffer( _target, 0 );
		return ptr;
	}

/*	void GLBuffer::sync( size_t offset, size_t length ) const
	{
		if( ! ( _access & GL_MAP_WRITE_BIT ) )
			return;
		glBindBuffer( _target, _buffer );
		glFlushMappedBufferRange( _target, ( GLintptr ) offset, ( GLsizeiptr ) length );
		glBindBuffer( _target, 0 );
	}*/

	void GLBuffer::unmap() const
	{
		glBindBuffer( _target, _buffer );
		glUnmapBuffer( _target );
		glBindBuffer( _target, 0 );
	}
}
