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

#include <cvt/gl/GLFBO.h>
#include <cvt/gl/GLRBO.h>
#include <cvt/gl/progs/GLDrawImageProg.h>

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

			GLSCamera& camera( int i ) { return _cams[ i ]; }
		private:
			std::vector<GLSCamera>		_cams;
			std::vector<GLSLight>		_lights;
			std::vector<GLSMaterial*>	_materials;
			std::vector<GLMesh*>		_meshes;
			GLSRenderableGroup*			_renderables;
			GLSShader					_shader;
			GLDrawImageProg				_drawimgp;


			GLSceneDrawFlags			_drawFlags;
			float						_fps;
			Time						_time;
			GLMesh						_mesh;
			GLTexture					_texture;
	};

	inline GLScene::GLScene()
	{
		_renderables = new GLSRenderableGroup();

		_texture.alloc( GL_DEPTH_COMPONENT, 640, 480, GL_DEPTH_COMPONENT, GL_FLOAT );
	}


	inline GLScene::GLScene( const Scene& scene ) :
		_mesh( *( ( SceneMesh* ) scene.geometry( 0 ) ) )
	{
		_renderables = new GLSRenderableGroup();

		for( size_t i = 0; i < scene.geometrySize(); i++ ) {
			if( scene.geometry( i )->type() == SCENEGEOMETRY_MESH ) {
				const SceneMaterial* mat;
				_meshes.push_back( new GLMesh( *( ( SceneMesh* ) scene.geometry( i ) ) ) );
				if( ( mat = scene.material( scene.geometry( i )->material() ) ) != NULL ) {
					_materials.push_back( new GLSMaterial( *mat, scene ) );
					_renderables->add( new GLSBaseModel( _meshes.back(), _materials.back() ) );
				} else
					_renderables->add( new GLSBaseModel( _meshes.back(), NULL ) );
			}
		}

		if( _cams.size() == 0 ) {
			_cams.push_back( GLSCamera( 80.0f, 1.333f, 0.1f, 150.0f ) );
			Matrix4f t;
			t.setIdentity();
			t *= 10.0f;
			t[ 3 ][ 3 ] = 1.0f;
			t[ 1 ][ 3 ] = 20.0f;
			t[ 2 ][ 3 ] = 180.0f;
			_cams.back().setTransformation( t );
		}

		_texture.alloc( GL_DEPTH_COMPONENT, 640, 480, GL_DEPTH_COMPONENT, GL_FLOAT );
	}

	inline GLScene::~GLScene()
	{
		delete _renderables;
	}

	inline void GLScene::draw( size_t cam )
	{
		GLFBO fbo( 640, 480 );
//		GLRBO rbo( GL_RGBA, 640, 480 );
//		GLTexture tex;
//		tex.alloc( GL_RGBA, 640, 480, GL_RGBA, GL_UNSIGNED_BYTE );

		_shader.setCamera( _cams[ cam ] );
		GLSRenderVisitor rvisitor( _shader );

		fbo.bind();
//		fbo.attach( GL_COLOR_ATTACHMENT0, tex );
		fbo.attach( GL_DEPTH_ATTACHMENT, _texture );
		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
		glEnable( GL_DEPTH_TEST );
		_renderables->accept( rvisitor );
		glDisable( GL_DEPTH_TEST );
		fbo.unbind();

		glEnable( GL_DEPTH_TEST );
		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
		_renderables->accept( rvisitor );
		glDisable( GL_DEPTH_TEST );

		Matrix4f proj;
		GL::orthoTranslation( proj, 0, ( float ) 640, 0, ( float ) 480, ( float ) 0, ( float ) 0, -100.0f, 100.0f, true );
		_drawimgp.bind();
		_drawimgp.setProjection( proj );
		_drawimgp.setAlpha( 1.0f );
//		GLTexture* _tex = _materials[ 0 ]->diffuseMap();
//		_drawimgp.drawImage( 0, 0, 320, 240, *_tex );
		_drawimgp.drawImage( 0, 0, 320, 240, _texture );
		_drawimgp.unbind();

	}

}

#endif
