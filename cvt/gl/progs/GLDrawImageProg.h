#ifndef CVT_GLDRAWIMAGEPROG_H
#define CVT_GLDRAWIMAGEPROG_H

#include <cvt/gl/OpenGL.h>
#include <cvt/gl/GLProgram.h>
#include <cvt/gl/GLVertexArray.h>
#include <cvt/gl/GLBuffer.h>
#include <cvt/gfx/Image.h>
#include <cvt/math/Matrix.h>
#include <cvt/geom/Rect.h>

namespace cvt {
	class GLDrawImageProg : private GLProgram
	{
		public:
			GLDrawImageProg();
			~GLDrawImageProg();

			using GLProgram::bind;
			using GLProgram::unbind;
			void setProjection( const Matrix4f& projection );
			void setAlpha( float alpha );
			void drawImage( int x, int y, const Image& img );
			void drawImage( int x, int y, int width, int height, const Image& img );

		private:
			GLVertexArray _vao;
			GLBuffer _vbo;
			GLBuffer _tvbo;
			GLint _mvploc;
			GLint _texloc;
	};


}

#endif
