#ifndef CVTGL_H
#define CVTGL_H

#define GL_GLEXT_PROTOTYPES
#include <GL/gl.h>
#include <GL/glext.h>

#include <GL/glx.h>

#include <iostream>
#include <string>
#include <vector>


namespace cvt {
	class GL {
		friend class ApplicationX11;

		public:
			static void GLVersion( unsigned int* major, unsigned int* minor );
			static void GLSLVersion( unsigned int* major, unsigned int* minor );
			static bool existsExtension( const std::string& extname );
			static void ( *getProcAddress( const char* str ) ) ();
			static void info( std::ostream& out );

			static void ( *glBindVertexArray )( GLuint array );
			static void ( *glDeleteVertexArrays )( GLsizei n, const GLuint *arrays );
			static void ( *glGenVertexArrays )( GLsizei n, GLuint *arrays);
			static GLboolean ( *glIsVertexArray )( GLuint array);

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
}

#endif
