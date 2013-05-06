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


			void paintEvent( PaintEvent& e, GFX& g );
			void paintGLEvent( PaintEvent& );

			void mousePressEvent( MousePressEvent& e );
			void mouseReleaseEvent( MouseReleaseEvent& e );
			void mouseMoveEvent( MouseMoveEvent& e);
			void resizeEvent( ResizeEvent& e );
			void keyPressEvent( KeyEvent& e );

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

	inline void GLSceneView::paintEvent( PaintEvent& e, GFX& g )
	{
		GLView::paintEvent( e, g );
		g.drawText( 10, 10, _scene.camera( 0 ).transformation().col( 3 ).toString().c_str() );
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


	inline void GLSceneView::keyPressEvent( KeyEvent& e )
	{
		Matrix4f t;

		switch( e.key() ) {
			case KEY_Left:
				 t.setRotationY( 5.0f / 180.0f * Math::PI );
				 _scene.camera( 0 ).transformation() *= t;
				 update();
				 break;
			case KEY_Right:
				 t.setIdentity();
				 t.setRotationY( -5.0f / 180.0f * Math::PI );
				 _scene.camera( 0 ).transformation() *= t;
				 update();
				 break;
			case KEY_W:
			case KEY_w:
				 t.setIdentity();
				 t[ 2 ][ 3 ] = -1.0f;
				 _scene.camera( 0 ).transformation() *= t;
				 update();
				 break;
			case KEY_S:
			case KEY_s:
				 t.setIdentity();
				 t[ 2 ][ 3 ] = 1.0f;
				 _scene.camera( 0 ).transformation() *= t;
				 update();
				 break;
			case KEY_Up:
				 t.setIdentity();
				 t[ 1 ][ 3 ] = 1.0f;
				 _scene.camera( 0 ).transformation() *= t;
				 update();
				 break;
			case KEY_Down:
				 t.setIdentity();
				 t[ 1 ][ 3 ] = -1.0f;
				 _scene.camera( 0 ).transformation() *= t;
				 update();
				 break;
			default:
				 break;
		}
	}
}

#endif
