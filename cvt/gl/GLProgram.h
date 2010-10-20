#ifndef CVTGLPROGRAM_H
#define CVTGLPROGRAM_H

#include <cvt/gl/OpenGL.h>
#include <cvt/gl/GLException.h>
#include <cvt/gfx/IFilterParameter.h>

namespace cvt {
	class GLProgram {
		public:
			GLProgram();
			~GLProgram();
			void build( const char* vertsrc, const char* fragsrc );
			void bind();
			void bindAttribLocation( const char *name, size_t location );
			void setArg( size_t n, IFilterParameter* p );
			void setArg( const char* name, IFilterParameter* p );
			void setArg( const char* name, int i );

		private:
			GLuint program;
	};
}

#endif
