#ifndef CVT_GLDRAWTEXTPROG_H
#define CVT_GLDRAWTEXTPROG_H

#include <cvt/gl/OpenGL.h>
#include <cvt/gl/GLProgram.h>
#include <cvt/gl/GLVertexArray.h>
#include <cvt/gl/GLBuffer.h>
#include <cvt/gl/GLTexture.h>
#include <cvt/math/Matrix.h>
#include <cvt/geom/Rect.h>

namespace cvt {
	class GLDrawTextProg : private GLProgram
	{
		public:
			GLDrawTextProg();
			~GLDrawTextProg();

			using GLProgram::bind;
			using GLProgram::unbind;
			void setProjection( const Matrix4f& projection );
			void setColor( const Color& color );
			void drawText( int x, int y, const char* txt );
			int  textWidth( const char* txt );

		private:
			GLVertexArray _vao;
			GLBuffer _vbo;
			GLTexture _tex;
			GLint _mvploc;
			GLint _texloc;
			GLint _scaleloc;
	};
}

#endif
