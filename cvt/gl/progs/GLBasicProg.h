#ifndef CVT_GLBASICPROG_H
#define CVT_GLBASICPROG_H

#include <cvt/gl/OpenGL.h>
#include <cvt/gl/GLProgram.h>
#include <cvt/gl/GLVertexArray.h>
#include <cvt/gl/GLBuffer.h>
#include <cvt/math/Matrix.h>
#include <cvt/geom/Rect.h>

namespace cvt {
	class GLBasicProg : private GLProgram
	{
		public:
			GLBasicProg();
			~GLBasicProg();

			using GLProgram::bind;
			using GLProgram::unbind;
			void setProjection( const Matrix4f& projection );
			void setColor( const Color& color );
			void fillRect( int x, int y, int w, int h );
			void fillRect( const Recti& rect );
			void drawLines( const Vector2f* pts, size_t n, float width );

		private:
			GLVertexArray _vao;
			GLBuffer _vbo;
			GLint _mvploc;
	};

	inline void GLBasicProg::fillRect( const Recti& rect )
	{
		fillRect( rect.x, rect.y, rect.width, rect.height );
	}

}

#endif
