/*
			CVT - Computer Vision Tools Library

 	 Copyright (c) 2012, Philipp Heise, Sebastian Klose

 	THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 	KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 	IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 	PARTICULAR PURPOSE.
 */
#ifndef CVT_GLSSHADER_H
#define CVT_GLSSHADER_H

#include <cvt/gl/progs/GLDrawModelProg.h>
#include <cvt/gl/scene/GLSCamera.h>
#include <cvt/gl/scene/GLSMaterial.h>

#include <cvt/util/Flags.h>

namespace cvt {
	enum GLSShaderMode {
		GLSSHADER_DEFAULT	   = 0,
		GLSSHADER_DEPTH        = 1,
		GLSSHADER_NORMAL	   = 2,
		GLSSHADER_DEPTH_NORMAL = 3
	};

	class GLSShader {
		friend class GLScene;
		public:
			GLSShader();
			~GLSShader() {}

			void setCamera( const GLSCamera& camera );
			void setTransformation( const Matrix4f& mat, bool setuniform = false );
			const Matrix4f& transformation() const { return _transformation; }
			void setMaterial( const GLSMaterial& mat );

			void setMode( GLSShaderMode mode );
			GLSShaderMode mode() const;

			void bind() { _prog.bind(); }
			void unbind() { _prog.unbind(); }

			void setUniforms();
		private:
			GLSShaderMode _mode;
			Matrix4f _proj;
			Matrix4f _transformation;
			//GLProgram* all programs for possible states
			GLDrawModelProg _prog;
	};

	inline GLSShader::GLSShader()
	{
		_prog.bind();
		_prog.setLightPosition( Vector3f( 1.0f, 1.0f, -2.0f ) );
		_prog.unbind();
	}

	inline void GLSShader::setCamera( const GLSCamera& camera )
	{
		_proj = camera.projection();
		_transformation = camera.transformation().inverse();
	}

	inline void GLSShader::setMode( GLSShaderMode mode )
	{
		_mode = mode;
	}

	inline GLSShaderMode GLSShader::mode() const
	{
		return _mode;
	}

	inline void GLSShader::setUniforms()
	{
		_prog.setProjection( _proj, _transformation );
	}

	inline void GLSShader::setTransformation( const Matrix4f& mat, bool setuniform )
	{
		_transformation = mat;
		if( setuniform ) {
			setUniforms();
		}
	}
}

#endif
