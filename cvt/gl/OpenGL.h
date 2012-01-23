#ifndef CVTGL_H
#define CVTGL_H

#ifndef GL_GLEXT_PROTOTYPES
	#define GL_GLEXT_PROTOTYPES
#endif

#ifdef APPLE
    #include <OpenGL/gl.h>
    #include <OpenGL/glext.h>
    #include <OpenGL/OpenGL.h>
#else
    #include <GL/gl.h>
	#include <GL/glext.h>
#endif


#include <cvt/math/Matrix.h>

#include <iostream>
#include <string>
#include <vector>


namespace cvt {

	/**
	  \ingroup GL
	  \defgroup GL OpenGL
	*/
	class GL {
		friend class ApplicationX11;
		friend class ApplicationOSX;

		public:
			static void GLVersion( unsigned int* major, unsigned int* minor );
			static void GLSLVersion( unsigned int* major, unsigned int* minor );
			static bool isGLSLVersionSupported( unsigned int major, unsigned int minor );
			static bool existsExtension( const String& extname );
			static void ( *getProcAddress( const char* str ) ) ();
			static void info( std::ostream& out );

			// VertexArray
			static void ( *glBindVertexArray )( GLuint array );
			static void ( *glDeleteVertexArrays )( GLsizei n, const GLuint *arrays );
			static void ( *glGenVertexArrays )( GLsizei n, GLuint *arrays);
			static GLboolean ( *glIsVertexArray )( GLuint array);

			// FramebufferObject
			static void ( *glBindRenderbuffer )( GLenum target, GLuint renderbuffer);
			static void ( *glDeleteRenderbuffers )( GLsizei n, const GLuint *renderbuffers);
			static void ( *glGenRenderbuffers )( GLsizei n, GLuint *renderbuffers);
			static void ( *glRenderbufferStorage )( GLenum target, GLenum internalformat, GLsizei width, GLsizei height);

			static void ( *glBindFramebuffer )( GLenum target, GLuint framebuffer );
			static void ( *glDeleteFramebuffers )( GLsizei n, const GLuint *framebuffers );
			static void ( *glGenFramebuffers )( GLsizei n, GLuint *framebuffers );

			static void ( *glFramebufferTexture1D )( GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level );
			static void ( *glFramebufferTexture2D )( GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level );
			static void ( *glFramebufferTexture3D )( GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level, GLint zoffset );

			static void ( *glFramebufferRenderbuffer )( GLenum target, GLenum attachment, GLenum renderbuffertarget, GLuint renderbuffer);


			static void ortho( Matrix4f& mat, float left, float right, float top, float bottom, float near = -1.0f, float far = 1.0f, bool fliph = false );
			static void orthoTranslation( Matrix4f& mat, float left, float right, float top, float bottom, float transx, float transy, float near = -1.0f, float far = 1.0f, bool fliph = false );
			static void frustum( Matrix4f& mat, float left, float right, float top, float bottom, float near, float far, bool fliph = false );
			static void perspective( Matrix4f& mat, float fovy, float aspect, float near, float far, bool fliph = false );
			static void subviewport( Matrix4f& mat, int x, int y, int width, int height, int viewportwidth, int viewportheight );

		private:
			static void init();
			GL();
			~GL();
			static void parseVersion( const char* str, unsigned int* major, unsigned int* minor );

			static unsigned int _glmajor, _glminor;
			static unsigned int _glslmajor, _glslminor;
			static std::vector<String> _extensions;
	};

	inline bool GL::isGLSLVersionSupported( unsigned int major, unsigned int minor )
	{
		return ( _glslmajor > major || ( _glslmajor == major && _glslminor >= minor ) );
	}

}

#endif
