#ifndef CVT_GLFILLROUNDRECTPROG_H
#define CVT_GLFILLROUNDRECTPROG_H

#include <cvt/gl/OpenGL.h>
#include <cvt/gl/GLProgram.h>
#include <cvt/gl/GLVertexArray.h>
#include <cvt/gl/GLBuffer.h>
#include <cvt/math/Matrix.h>
#include <cvt/geom/Rect.h>

namespace cvt {
	class GLFillRoundRectProg : private GLProgram
	{
		public:
			GLFillRoundRectProg();
			~GLFillRoundRectProg();

			using GLProgram::bind;
			using GLProgram::unbind;
			void setProjection( const Matrix4f& projection );
			void setColor( const Color& color );
			void fillRoundRect( int x, int y, int w, int h, float radius );
			void fillRoundRect( const Recti& rect, float radius );

		private:
			GLVertexArray _vao;
			GLBuffer _vbo;
			GLBuffer _tvbo;
			GLint _mvploc;
			GLint _radiusloc;
			GLint _sizeloc;
	};

	inline void GLFillRoundRectProg::fillRoundRect( const Recti& rect, float radius )
	{
		fillRoundRect( rect.x, rect.y, rect.width, rect.height, radius );
	}

}

#endif
