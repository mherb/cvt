/*
			CVT - Computer Vision Tools Library

 	 Copyright (c) 2012, Philipp Heise, Sebastian Klose

 	THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 	KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 	IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 	PARTICULAR PURPOSE.
 */
#ifndef CVT_GLFILLPATHPROG_H
#define CVT_GLFILLPATHPROG_H

#include <cvt/gl/OpenGL.h>
#include <cvt/gl/GLProgram.h>
#include <cvt/gl/GLModel.h>
#include <cvt/math/Matrix.h>
#include <cvt/geom/Rect.h>
#include <cvt/gfx/GFX.h>
#include <cvt/gfx/Path.h>

namespace cvt {
	class GLFillPathProg : private GLProgram
	{
		public:
			GLFillPathProg();
			~GLFillPathProg();

			using GLProgram::bind;
			using GLProgram::unbind;
			void setProjection( const Matrix4f& projection );
			void setColor( const Color& color );
			void fillPath( const Pathf& path, GFX::PolygonFillRule rule );
			void fillPath( const Pathd& path, GFX::PolygonFillRule rule );

		private:
			GLVertexArray _vao;
			GLint _mvploc;
	};
}

#endif
