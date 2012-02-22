/*
			CVT - Computer Vision Tools Library

 	 Copyright (c) 2012, Philipp Heise, Sebastian Klose

 	THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 	KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 	IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 	PARTICULAR PURPOSE.
 */
#ifndef CVT_GLDRAWICONPROG_H
#define CVT_GLDRAWICONPROG_H

#include <cvt/gl/OpenGL.h>
#include <cvt/gl/GLProgram.h>
#include <cvt/gl/GLVertexArray.h>
#include <cvt/gl/GLBuffer.h>
#include <cvt/gl/GLTexture.h>
#include <cvt/math/Matrix.h>
#include <cvt/math/Vector.h>
#include <cvt/geom/Rect.h>

namespace cvt {
	class GLDrawIconProg : private GLProgram
	{
		public:
			GLDrawIconProg();
			~GLDrawIconProg();

			using GLProgram::bind;
			using GLProgram::unbind;
			void setProjection( const Matrix4f& projection );
			void setColor( const Color& color );
			void drawIcon( int x, int y, int icon );
			void drawIcons( const Vector2i* pts, size_t npts, int icon );
			void drawIcons( const Vector2f* pts, size_t npts, int icon );

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
