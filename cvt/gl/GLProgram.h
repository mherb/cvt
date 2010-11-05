#ifndef CVTGLPROGRAM_H
#define CVTGLPROGRAM_H

#include <cvt/gl/OpenGL.h>
#include <cvt/gl/GLException.h>
#include <cvt/gfx/IFilterParameter.h>

namespace cvt {

#define GLSL_VERTEX_IDX   0
#define GLSL_COLOR_IDX    1
#define GLSL_NORMAL_IDX   2
#define GLSL_TEXCOORD_IDX 3

	class GLProgram {
		public:
			GLProgram();
			~GLProgram();
			void build( const char* vertsrc, const char* fragsrc );
			void bind();
			void unbind();
			void bindAttribLocation( const char *name, unsigned int location );
			unsigned int getAttribLocation( const char* name );
			void setArg( size_t n, IFilterParameter* p );
			void setArg( const char* name, IFilterParameter* p );
			void setArg( const char* name, int i );
			void setArg( const char* name, float f );

		private:
			GLuint program;
	};
}

#endif