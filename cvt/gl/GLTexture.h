#ifndef CVTGLTEXTURE_H
#define CVTGLTEXTURE_H

#include <cvt/gl/OpenGL.h>

namespace cvt {
	class GLFBO;

	class GLTexture {
		friend class GLFBO;

		public:
			GLTexture( GLenum target = GL_TEXTURE_2D );
			~GLTexture();
			void bind() const;
			void unbind() const;
			GLenum target() const;
			void size( GLsizei& width, GLsizei& height ) const;
			GLsizei width() const;
			GLsizei height() const;
			void alloc( GLint internalFormat, GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid* data = NULL, size_t stride = 0 );
//			void alloc( const Image& img, bool copy = false );
			void setData( GLint	xoffset, GLint yoffset,	GLsizei	width, GLsizei height, GLenum format, GLenum type, const GLvoid* data, size_t stride = 0 );
//			void setData( const GLBuffer& buffer, GLint	xoffset, GLint yoffset,	GLsizei	width, GLsizei height, GLenum format, GLenum type, const GLvoid* data, size_t stride = 0 );

		private:
			GLTexture( const GLTexture& );
			GLTexture& operator=( const GLTexture& );

			GLuint _tex;
			GLenum _target;
			GLint _internalFormat;
			GLsizei _width, _height;
	};

	inline GLenum GLTexture::target() const
	{
		return _target;
	}


	inline void GLTexture::size( GLsizei& width, GLsizei& height ) const
	{
		width = _width;
		height = _height;
	}

	inline size_t GLTexture::width() const
	{
		return _width;
	}

	inline size_t GLTexture::height() const
	{
		return _height;
	}

}

#endif

