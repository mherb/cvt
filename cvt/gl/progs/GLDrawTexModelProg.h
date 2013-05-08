/*
			CVT - Computer Vision Tools Library

 	 Copyright (c) 2012, Philipp Heise, Sebastian Klose

 	THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 	KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 	IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 	PARTICULAR PURPOSE.
 */
#ifndef CVT_GLDRAWTEXMODELPROG_H
#define CVT_GLDRAWTEXMODELPROG_H

#include <cvt/gl/OpenGL.h>
#include <cvt/gl/GLProgram.h>
#include <cvt/math/Matrix.h>
#include <cvt/geom/Rect.h>

namespace cvt {
	class GLDrawTexModelProg : private GLProgram
	{
		public:
			GLDrawTexModelProg();
			~GLDrawTexModelProg();

			using GLProgram::bind;
			using GLProgram::unbind;
			void setLightPosition( const Vector3f& position );
			void setProjection( const Matrix4f& projection, const Matrix4f& modelview );

		private:
			GLint _mvploc;
			GLint _normmloc;
			GLint _lightposloc;
			GLint _texloc;
	};

}

#endif
