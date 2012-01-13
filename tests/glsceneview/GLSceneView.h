#ifndef CVT_GLSCENEVIEW_H
#define CVT_GLSCENEVIEW_H

#include <cvt/gui/GLView.h>
#include <cvt/gl/GLMesh.h>
#include <cvt/gl/GLProgram.h>
#include <cvt/gl/progs/GLDrawModelProg.h>

#include <cvt/geom/scene/Scene.h>

namespace cvt {
	class GLSceneView : public GLView
	{
		public:
			GLSceneView( const SceneMesh& mesh );
			~GLSceneView();

			void paintGLEvent( PaintEvent* );

		private:
//			GLProgram _prog;
			GLDrawModelProg _prog;
			GLMesh _mesh;
	};

	inline GLSceneView::GLSceneView( const SceneMesh& mesh ) : _mesh( mesh )
	{
//		_prog.build(  );
	}

	inline GLSceneView::~GLSceneView()
	{
	}

	/*inline GLScene* GLSceneView::scene()
	{
		return _scene;
	}*/

	inline void GLSceneView::paintGLEvent( PaintEvent* )
	{
		setViewport( rect() );
		_prog.bind();

		{
			Matrix4f proj, tmp;
			GL::perspective( proj, 60.0f, ( float ) rect().width / ( float ) rect().height, 0.01f, 100.0f );
			tmp.setIdentity();
			tmp[ 2 ][ 3 ] = -2.5f;

			_prog.setLightPosition( Vector3f( 1.0f, 1.0f, -1.0f ) );
			_prog.setProjection( proj, tmp );

			_mesh.draw();
		}

		_prog.unbind();
	}
}

#endif
