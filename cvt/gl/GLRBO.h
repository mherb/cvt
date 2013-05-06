/*
			CVT - Computer Vision Tools Library

 	 Copyright (c) 2012, Philipp Heise, Sebastian Klose

 	THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 	KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 	IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 	PARTICULAR PURPOSE.
 */
#ifndef CVT_GLRBO_H
#define CVT_GLRBO_H

#include <cvt/gl/OpenGL.h>

namespace cvt {
	class GLFBO;

	/**
	  \ingroup GL
	*/
	class GLRBO {
		friend class GLFBO;

		public:
			GLRBO( GLenum format, GLsizei width, GLsizei height );
			~GLRBO();

			GLenum format() const;
			GLsizei width() const;
			GLsizei height() const;
			void size( GLsizei& width, GLsizei& height ) const;

		private:
			GLRBO( const GLRBO& );
			GLRBO& operator=( const GLRBO& );

			GLuint _rbo;
			GLenum _format;
			GLsizei _width, _height;
	};

	inline GLRBO::GLRBO( GLenum format, GLsizei width, GLsizei height ) : _rbo( 0 ), _format( format ), _width( width ), _height( height )
	{
		GL::glGenRenderbuffers( 1, &_rbo );
		GL::glBindRenderbuffer( GL_RENDERBUFFER, _rbo );
		GL::glRenderbufferStorage( GL_RENDERBUFFER, _format, _width, _height );
		GL::glBindRenderbuffer( GL_RENDERBUFFER, 0 );
	}

	inline GLRBO::~GLRBO()
	{
		GL::glDeleteRenderbuffers( 1, &_rbo );
	}

	inline GLenum GLRBO::format() const
	{
		return _format;
	}

	inline GLsizei GLRBO::width() const
	{
		return _width;
	}

	inline GLsizei GLRBO::height() const
	{
		return _height;
	}

	inline void GLRBO::size( GLsizei& width, GLsizei& height ) const
	{
		width = _width;
		height = _height;
	}
}


#endif
