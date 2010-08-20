#include "gui/GLImage.h"

namespace cvt {

	GLImage::GLImage() : _iw( 1.0f ), _ih( 1.0f )
	{
		glGenTextures( 1, &_tex );
		glBindTexture( GL_TEXTURE_2D, _tex );
		glPixelStorei( GL_UNPACK_ALIGNMENT, 1 );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	}

	GLImage::GLImage( const Image& i ) : _iw( 1.0f ), _ih( 1.0f )
	{
		glGenTextures( 1, &_tex );
		glBindTexture( GL_TEXTURE_2D, _tex );
		glPixelStorei( GL_UNPACK_ALIGNMENT, 1 );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		setImage( i );
	}

	GLImage::~GLImage()
	{
		glDeleteTextures( 1, &_tex );
	}

	void GLImage::draw() const
	{
		glBindTexture( GL_TEXTURE_2D, _tex );
		glBegin( GL_QUADS );
			glTexCoord2f( 0.0f, 0.0f );
			glVertex2i( _rect.x, _rect.y );
			glTexCoord2f( ( float ) _rect.width / _iw, 0.0f );
			glVertex2i( _rect.x + _rect.width, _rect.y );
			glTexCoord2f( ( float ) _rect.width / _iw, ( float ) _rect.height / _ih );
			glVertex2i( _rect.x + _rect.width, _rect.y + _rect.height );
			glTexCoord2f( 0.0f, ( float ) _rect.height / _ih );
			glVertex2i( _rect.x, _rect.y + _rect.height );
		glEnd();
	}

	void GLImage::setImage( const Image& img )
	{
		GLenum format;

		_iw = ( float ) img.width();
		_ih = ( float ) img.height();

		glBindTexture( GL_TEXTURE_2D, _tex );
		switch( img.order() ) {
			case CVT_GRAY: format = GL_LUMINANCE; break;
			case CVT_GRAYALPHA: format = GL_LUMINANCE_ALPHA; break;
			case CVT_RGBA: format = GL_RGBA; break;
			case CVT_BGRA: format = GL_BGRA; break;
		}
		//		glPixelStorei( GL_UNPACK_ROW_LENGTH, ( GLint ) img.stride() );
				glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, ( GLsizei ) img.width(), ( GLsizei ) img.height(), 0, format, (img.type()==CVT_UBYTE)?GL_UNSIGNED_BYTE:GL_FLOAT, ( void* ) img.data() );
//		glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, img.width(), img.height(), 0, GL_BGRA, GL_UNSIGNED_BYTE, img.data() );
	}
}
