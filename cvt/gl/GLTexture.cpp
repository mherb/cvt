#include <cvt/gl/GLTexture.h>

namespace cvt {

		GLTexture::GLTexture( GLenum target ) : _target( target )
		{
			glGenTextures( 1, &_tex );
		}

		GLTexture::~GLTexture()
		{
			glDeleteTextures( 1, &_tex );
		}

		void GLTexture::bind() const
		{
			glBindTexture( _target, _tex );
		}

		void GLTexture::unbind() const
		{
			glBindTexture( _target, 0 );
		}

}
