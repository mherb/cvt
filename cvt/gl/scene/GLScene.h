/*
			CVT - Computer Vision Tools Library

 	 Copyright (c) 2012, Philipp Heise, Sebastian Klose

 	THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 	KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 	IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 	PARTICULAR PURPOSE.
 */
#ifndef CVT_GLSCENE_H
#define CVT_GLSCENE_H


#include <cvt/gl/scene/GLSCamera.h>
#include <cvt/gl/scene/GLSLight.h>
#include <cvt/gl/scene/GLSMaterial.h>
#include <cvt/gl/scene/GLSRenderable.h>
#include <cvt/gl/scene/GLSRenderableGroup.h>
#include <cvt/gl/scene/GLSBaseModel.h>
#include <cvt/gl/scene/GLSVisitor.h>
#include <cvt/gl/scene/GLSShader.h>
#include <cvt/gl/GLMesh.h>
#include <cvt/util/Time.h>

namespace cvt {
	enum GLSceneDrawFeatures {
		GLS_SHOW_FPS	 = ( 1 << 0 ),
		GLS_SHADOWS		 = ( 1 << 1 ),
		GLS_SSAO		 = ( 1 << 2 ),
		GLS_DEBUG		 = ( 1 << 3 )
	};

	CVT_ENUM_TO_FLAGS( GLSceneDrawFeatures, GLSceneDrawFlags )

	class GLScene
	{
		public:
			GLScene();
			GLScene( const Scene& scene );
			~GLScene();


			void draw( size_t cam );
			GLSceneDrawFlags& drawMode() { return _drawFlags; }
			const GLSceneDrawFlags& drawMode() const { return _drawFlags; }

			const Time& time() const { return _time; }
		private:
			std::vector<GLSCamera>		_cams;
			std::vector<GLSLight>		_lights;
			std::vector<GLSMaterial>	_materials;
			std::vector<GLTexture>		_textures;
			std::vector<GLMesh>			_meshes;
			GLSRenderableGroup*			_renderables;
			GLSShader					_shader;

			GLSceneDrawFlags			_drawFlags;
			float						_fps;
			Time						_time;
			GLMesh						_mesh;
	};

	inline GLScene::GLScene()
	{
		_renderables = new GLSRenderableGroup();
	}


	inline GLScene::GLScene( const Scene& scene ) :
		_mesh( *( ( SceneMesh* ) scene.geometry( 0 ) ) )
	{
#if 0
		_renderables = new GLSRenderableGroup();

		for( size_t i = 0; i < scene.geometrySize(); i++ ) {
			if( scene.geometry( i )->type() == SCENEGEOMETRY_MESH ) {
				_meshes.push_back( GLMesh( *( ( SceneMesh* ) scene.geometry( i ) ) ) );
				_renderables->add( new GLSBaseModel( &_meshes.back(), NULL ) );
			}
		}

		if( _cams.size() == 0 ) {
			_cams.push_back( GLSCamera( 60.0f, 1.333f, 1.0f, 100.0f ) );
			Matrix4f t;
			t.setIdentity();
			//t *= 10.0f;
			//t[ 3 ][ 3 ] = 1.0f;
			t[ 2 ][ 3 ] = 5.0f;
			_cams.back().setTransformation( t );
		}
#endif
	}

	inline GLScene::~GLScene()
	{
//		delete _renderables;
	}

	inline void GLScene::draw( size_t cam )
	{
//		_shader.setCamera( _cams[ cam ] );
//		glClearColor( 1.0f, 1.0f, 0.0f, 1.0f );
//		glClearDepth( 0.0f );
//		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

//		GLSRenderVisitor rvisitor( _shader );
		_shader._prog.bind();

		float _scale = 0.25f;

			Matrix4f proj, tmp;
			GL::perspective( proj, 60.0f, 1.0f, 0.01f, 100.0f );
			tmp.setIdentity();
			tmp[ 2 ][ 3 ] = -5.5f;
			Matrix4f diag;
			diag.setDiagonal( Vector4f( _scale, _scale, _scale, 1.0f ) );
			tmp *= diag;

			_shader._prog.setLightPosition( Vector3f( 1.0f, 1.0f, -1.0f ) );
			_shader._prog.setProjection( proj, tmp );


		_shader.setUniforms();
		glEnable( GL_DEPTH_TEST );
		_mesh.draw();

		//_renderables->accept( rvisitor );
	glDisable( GL_DEPTH_TEST );

		_shader._prog.unbind();
	}

}

#endif
