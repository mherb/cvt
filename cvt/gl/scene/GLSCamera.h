/*
			CVT - Computer Vision Tools Library

 	 Copyright (c) 2012, Philipp Heise, Sebastian Klose

 	THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 	KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 	IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 	PARTICULAR PURPOSE.
 */
#ifndef CVT_GLSCAMERA_H
#define CVT_GLSCAMERA_H


#include <cvt/gl/OpenGL.h>
#include <cvt/gl/scene/GLSTransformable.h>

namespace cvt {
	class GLSCamera : public GLSTransformable {
		public:
			GLSCamera( const GLSCamera& cam );
			GLSCamera( float fovy, float aspect, float near, float far );
			~GLSCamera();

			const Matrix4f& projection() const { return _proj; }

		private:
			Matrix4f _proj;
	};

	inline GLSCamera::GLSCamera( const GLSCamera& cam ) : GLSTransformable( cam ), _proj( cam._proj )
	{
	}

	inline GLSCamera::GLSCamera( float fovy, float aspect, float near, float far )
	{
		GL::perspective( _proj, fovy, aspect, near, far );
	}

	inline GLSCamera::~GLSCamera()
	{
	}
}

#endif
