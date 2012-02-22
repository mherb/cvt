/*
			CVT - Computer Vision Tools Library

 	 Copyright (c) 2012, Philipp Heise, Sebastian Klose

 	THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 	KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 	IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 	PARTICULAR PURPOSE.
 */
#include "gui/GLImage.h"

namespace cvt {

	GLImage::GLImage() : _iw( 1.0f ), _ih( 1.0f )
	{
		glGenTextures( 1, &_tex );
		glBindTexture( GL_TEXTURE_2D, _tex );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	}

	GLImage::GLImage( const Image& i ) : _iw( 1.0f ), _ih( 1.0f )
	{
		glGenTextures( 1, &_tex );
		glBindTexture( GL_TEXTURE_2D, _tex );
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
			glTexCoord2f( 1.0f, 0.0f );
			glVertex2i( _rect.x + _rect.width, _rect.y );
			glTexCoord2f( 1.0f, 1.0f );
			glVertex2i( _rect.x + _rect.width, _rect.y + _rect.height );
			glTexCoord2f( 0.0f, 1.0f );
			glVertex2i( _rect.x, _rect.y + _rect.height );
		glEnd();
	}

	void GLImage::setImage( const Image& img )
	{
		GLenum format;

		_iw = ( float ) img.width();
		_ih = ( float ) img.height();

		glBindTexture( GL_TEXTURE_2D, _tex );
		switch( img.order().id ) {
			case ICHANNELORDER_GRAY: format = GL_LUMINANCE; break;
			case ICHANNELORDER_GRAYALPHA: format = GL_LUMINANCE_ALPHA; break;
			case ICHANNELORDER_RGBA: format = GL_RGBA; break;
			case ICHANNELORDER_BGRA: format = GL_BGRA; break;
		}
		glPixelStorei( GL_UNPACK_ALIGNMENT, ( img.bpc() >> 3 ) );
		glPixelStorei( GL_UNPACK_ROW_LENGTH, ( GLint ) img.stride() / ( img.bpp() ) );
		glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, ( GLsizei ) img.width(), ( GLsizei ) img.height(), 0, format, (img.type()==IType::UBYTE)?GL_UNSIGNED_BYTE:GL_FLOAT, ( void* ) img.map() );
		img.unmap();
		updated();
	}
}
