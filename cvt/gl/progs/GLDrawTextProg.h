/*
			CVT - Computer Vision Tools Library

 	 Copyright (c) 2012, Philipp Heise, Sebastian Klose

 	THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 	KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 	IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 	PARTICULAR PURPOSE.
 */
#ifndef CVT_GLDRAWTEXTPROG_H
#define CVT_GLDRAWTEXTPROG_H

#include <cvt/gl/OpenGL.h>
#include <cvt/gl/GLProgram.h>
#include <cvt/gl/GLVertexArray.h>
#include <cvt/gl/GLBuffer.h>
#include <cvt/gl/GLTexture.h>
#include <cvt/math/Matrix.h>
#include <cvt/geom/Rect.h>
#include <cvt/gl/GLTexFont.h>

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
			void drawText( int x, int y, const char* txt, const GLTexFont& glfont );

		private:
			GLVertexArray _vao;
			GLBuffer _vbo;
			GLint _mvploc;
			GLint _texloc;
			GLint _scaleloc;
	};
}

#endif
