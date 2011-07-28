#ifndef CVT_GLRBO_H
#define CVT_GLRBO_H

#include <cvt/gl/OpenGL.h>

namespace cvt {
	class GLFBO;

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
		glGenRenderbuffers( 1, &_rbo );
		glBindRenderbuffer( GL_RENDERBUFFER_EXT, _rbo );
		glRenderbufferStorage( GL_RENDERBUFFER, _format, _width, _height );
		glBindRenderbuffer( GL_RENDERBUFFER, 0 );
	}

	inline GLRBO::~GLRBO()
	{
		glDeleteRenderbuffers( 1, &_rbo );
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
