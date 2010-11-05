#include <cvt/gfx/ImageAllocatorGL.h>
#include <cvt/util/SIMD.h>

#include <stdio.h>
#include <execinfo.h>

namespace cvt {

	ImageAllocatorGL::ImageAllocatorGL() : ImageAllocator(), _ptr( NULL ), _ptrcount( 0 ), _dirty( false )
	{
		glGenTextures( 1, &_tex2d );
		glGenBuffers( 1, &_glbuf );
	}

	ImageAllocatorGL::~ImageAllocatorGL()
	{
		glDeleteTextures( 1, &_tex2d );
		glDeleteBuffers( 1, &_glbuf );
	}

	void ImageAllocatorGL::alloc( size_t width, size_t height, const IOrder order, const IType type )
	{
		GLenum glformat, gltype;

		_width = width;
		_height = height;
		_order = order;
		_type = type;
		_stride = Math::pad16( _width * _order.channels* _type.size );
		_size = _stride * _height;

		glBindBuffer( GL_PIXEL_UNPACK_BUFFER, _glbuf );
		glBufferData( GL_PIXEL_UNPACK_BUFFER, _size, NULL, GL_DYNAMIC_DRAW );
		glBindBuffer( GL_PIXEL_UNPACK_BUFFER, 0 );

		glBindTexture( GL_TEXTURE_2D, _tex2d );
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glPixelStorei( GL_UNPACK_ROW_LENGTH, ( GLint ) ( _stride / ( _type.size * _order.channels ) ) );
		getGLFormat( _order, _type, glformat, gltype );
		/* do not copy non-meaningful PBO content - just allocate space, since current PBO content is undefined */
		glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, ( GLsizei ) _width, ( GLsizei ) _height, 0, glformat, gltype, NULL );
	}

	void ImageAllocatorGL::copy( const ImageAllocator* x, const Recti* r = NULL )
	{
		const uint8_t* src;
		const uint8_t* osrc;
		size_t sstride, dstride;
		uint8_t* dst;
		uint8_t* odst;
		size_t i, n;
		Recti rect( 0, 0, ( int ) x->_width, ( int ) x->_height );
		SIMD* simd = SIMD::get();

		if( r )
			rect.intersect( *r );
		alloc( rect.width, rect.height, x->_order, x->_type );

		osrc = src = x->map( &sstride );
		src += rect.y * sstride + x->_type.size * x->_order.channels * rect.x;
		odst = dst = map( &dstride );
		n =  x->_type.size * x->_order.channels * rect.width;

		i = rect.height;
		while( i-- ) {
			simd->Memcpy( dst, src, n );
			dst += sstride;
			src += dstride;
		}
		x->unmap( osrc );
		unmap( odst );
	}

	uint8_t* ImageAllocatorGL::map( size_t* stride )
	{
		uint8_t* ptr;

		*stride = _stride;
		if( !_ptrcount ) {
			glBindBuffer( GL_PIXEL_UNPACK_BUFFER, _glbuf );
			_ptr = ( uint8_t* ) glMapBufferRange( GL_PIXEL_UNPACK_BUFFER, 0, _size, GL_MAP_READ_BIT | GL_MAP_WRITE_BIT | GL_MAP_FLUSH_EXPLICIT_BIT  );
			glBindBuffer( GL_PIXEL_UNPACK_BUFFER, 0 );
		}
		_ptrcount++;
		_dirty = true;
		return ( uint8_t* ) _ptr;
	}

	const uint8_t* ImageAllocatorGL::map( size_t* stride ) const
	{
		uint8_t* ptr;

		*stride = _stride;
		if( !_ptrcount ) {
			glBindBuffer( GL_PIXEL_UNPACK_BUFFER, _glbuf );
			_ptr = ( uint8_t* ) glMapBufferRange( GL_PIXEL_UNPACK_BUFFER, 0, _size, GL_MAP_READ_BIT | GL_MAP_WRITE_BIT | GL_MAP_FLUSH_EXPLICIT_BIT  );
			glBindBuffer( GL_PIXEL_UNPACK_BUFFER, 0 );
		}
		_ptrcount++;
		return ( uint8_t* ) _ptr;
	}

	void ImageAllocatorGL::unmap( const uint8_t* ptr ) const
	{
		_ptrcount--;
		if( !_ptrcount ) {
			glBindBuffer( GL_PIXEL_UNPACK_BUFFER, _glbuf );
			if( _dirty ) {
				GLenum glformat, gltype;

				glFlushMappedBufferRange( GL_PIXEL_UNPACK_BUFFER, 0, _size );
				glUnmapBuffer( GL_PIXEL_UNPACK_BUFFER );

				glBindTexture( GL_TEXTURE_2D, _tex2d );
				glPixelStorei( GL_UNPACK_ROW_LENGTH, ( GLint ) ( _stride / ( _type.size * _order.channels ) ) );
				getGLFormat( _order, _type, glformat, gltype );
				glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, ( GLsizei ) _width, ( GLsizei ) _height, glformat, gltype, NULL);
				_dirty = false;
			} else {
				glUnmapBuffer( GL_PIXEL_UNPACK_BUFFER );
			}
			glBindBuffer( GL_PIXEL_UNPACK_BUFFER, 0 );
			_ptr = NULL;
		}
	}


	void ImageAllocatorGL::getGLFormat( IOrder order, IType type, GLenum& glformat, GLenum& gltype ) const
	{
		switch( order.id ) {
			case ICHANNELORDER_GRAY: glformat = GL_RED; break;
			case ICHANNELORDER_GRAYALPHA: glformat =GL_RG; break;
			case ICHANNELORDER_RGBA:	glformat = GL_RGBA; break;
			case ICHANNELORDER_BGRA:	glformat = GL_BGRA; break;
		}

		switch( type.id ) {
			case ICHANNELTYPE_UBYTE: gltype = GL_UNSIGNED_BYTE; break;
			case ICHANNELTYPE_FLOAT: gltype = GL_FLOAT; break;
		}
	}
}
