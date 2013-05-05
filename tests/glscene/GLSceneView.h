#ifndef CVT_GLSCENEVIEW_H
#define CVT_GLSCENEVIEW_H

#include <cvt/gui/GLView.h>
#include <cvt/geom/ArcBall.h>
#include <cvt/gl/scene/GLScene.h>

namespace cvt {
	class GLSceneView : public GLView
	{
		public:
			GLSceneView( GLScene& scene );
			~GLSceneView();

			void paintGLEvent( PaintEvent& );

			void mousePressEvent( MousePressEvent& e );
			void mouseReleaseEvent( MouseReleaseEvent& e );
			void mouseMoveEvent( MouseMoveEvent& e);
			void resizeEvent( ResizeEvent& e );

		private:
			GLScene&		_scene;
			ArcBall			_arcball;
			Matrix4f		_transformation;
			float			_scale;
			Vector2i		_mousepress;
	};

	inline GLSceneView::GLSceneView( GLScene& scene ) : _scene( scene )
	{
	}

	inline GLSceneView::~GLSceneView()
	{
	}


	inline void GLSceneView::paintGLEvent( PaintEvent& )
	{
		setViewport( rect() );
		_scene.draw( 0 );
	}


	inline void GLSceneView::resizeEvent( ResizeEvent& e )
	{
		_arcball.setViewportSize( e.width(), e.height() );
	}



	inline void GLSceneView::mousePressEvent( MousePressEvent& e )
	{
		if( e.button() == 1 ) {
			_mousepress.x = e.x;
			_mousepress.y = e.y;
		}
	}


	inline void GLSceneView::mouseReleaseEvent( MouseReleaseEvent& e )
	{
		if( e.button() == 4 ) {
			_scale += 0.05f;
			update();
		} else if( e.button() == 5 ) {
			_scale -= 0.05f;
			update();
		}
	}

	inline void GLSceneView::mouseMoveEvent( MouseMoveEvent& e )
	{
		if( e.buttonMask() & 1 ) {
			Matrix4f rot;
			_arcball.getRotation( rot, _mousepress.x, _mousepress.y, e.x, e.y );

			_transformation = rot * _transformation;

			_mousepress.x = e.x;
			_mousepress.y = e.y;

			update();
		}
	}
}

#endif
