#ifndef CVTGL_H
#define CVTGL_H

#define GL_GLEXT_PROTOTYPES
#include <GL/gl.h>
#include <GL/glext.h>

#include <cvt/math/Matrix.h>

#include <iostream>
#include <string>
#include <vector>


namespace cvt {
	class GL {
		friend class ApplicationX11;

		public:
			static void GLVersion( unsigned int* major, unsigned int* minor );
			static void GLSLVersion( unsigned int* major, unsigned int* minor );
			static bool isGLSLVersionSupported( unsigned int major, unsigned int minor );
			static bool existsExtension( const std::string& extname );
			static void ( *getProcAddress( const char* str ) ) ();
			static void info( std::ostream& out );

			static void ( *glBindVertexArray )( GLuint array );
			static void ( *glDeleteVertexArrays )( GLsizei n, const GLuint *arrays );
			static void ( *glGenVertexArrays )( GLsizei n, GLuint *arrays);
			static GLboolean ( *glIsVertexArray )( GLuint array);

			static void ortho( Matrix4f& mat, float left, float right, float top, float bottom, float near = -1.0f, float far = 1.0f );
			static void orthoTranslation( Matrix4f& mat, float left, float right, float top, float bottom, float transx, float transy, float near = -1.0f, float far = 1.0f );
			static void frustum( Matrix4f& mat, float left, float right, float top, float bottom, float near, float far );
			static void perspective( Matrix4f& mat, float fovy, float aspect, float near, float far );
			static void subviewport( Matrix4f& mat, int x, int y, int width, int height, int viewportwidth, int viewportheight );

		private:
			static void init();
			GL();
			~GL();
			static void parseVersion( const char* str, unsigned int* major, unsigned int* minor );
			static void parseExtensions( const char* str );

			static unsigned int _glmajor, _glminor;
			static unsigned int _glslmajor, _glslminor;
			static std::vector<std::string*> _extensions;
	};

	inline bool GL::isGLSLVersionSupported( unsigned int major, unsigned int minor )
	{
		return ( _glslmajor > major || ( _glslmajor == major && _glslminor >= minor ) );
	}

}

#endif
