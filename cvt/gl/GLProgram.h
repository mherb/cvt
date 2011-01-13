#ifndef CVTGLPROGRAM_H
#define CVTGLPROGRAM_H

#include <cvt/gl/OpenGL.h>
#include <cvt/gl/GLException.h>
#include <cvt/math/Matrix.h>
#include <cvt/gfx/Color.h>

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
			unsigned int getAttribLocation( const char* name ) const;
			GLint uniformLocation( const char* name ) const;

			void setArg( const char* name, int i );
			void setArg( const char* name, float f );
			void setArg( const char* name, float f1, float f2 );
			void setArg( const char* name, const Color & c );
			void setArg( const char* name, const Matrix4f & m );

		private:
			GLuint program;
	};
}

#endif
