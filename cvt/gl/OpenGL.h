#ifndef CVTGL_H
#define CVTGL_H

#define GL_GLEXT_PROTOTYPES
#include <GL/gl.h>
#include <GL/glext.h>

#include <iostream>
#include <string>
#include <vector>

namespace cvt {
	class GL {

		public:
			static bool init();
			static void GLVersion( unsigned int* major, unsigned int* minor );
			static void GLSLVersion( unsigned int* major, unsigned int* minor );
			static void info( std::ostream& out );

		private:
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
