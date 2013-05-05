#ifndef CVT_GLSCENEVIEW_H
#define CVT_GLSCENEVIEW_H

#include <cvt/gui/GLView.h>
#include <cvt/gl/GLMesh.h>
#include <cvt/gl/GLProgram.h>
#include <cvt/gl/progs/GLBasicProg.h>
#include <cvt/geom/ArcBall.h>
#include <cvt/geom/scene/Scene.h>
#include <cvt/geom/scene/ScenePoints.h>
#include <cvt/gl/GLPoints.h>
#include <cvt/gl/GLLines.h>
#include <cvt/io/VideoInput.h>

namespace cvt {
	class GLSceneView : public GLView
	{
		public:
                        GLSceneView();
			~GLSceneView();

			void paintGLEvent( PaintEvent& );
			void mousePressEvent( MousePressEvent& e );
			void mouseReleaseEvent( MouseReleaseEvent& e );
			void mouseMoveEvent( MouseMoveEvent& e);
			void resizeEvent( ResizeEvent& e );

                        void setScenePoints( const ScenePoints& pts );

                private:
                        float                       _scale;
                        GLBasicProg                 _prog;
                        GLPoints                    _points;
                        ArcBall                     _arcball;
                        Matrix4f                    _transformation;
                        Vector2i                    _mousepress;
	};

        inline GLSceneView::GLSceneView() :
            _scale( 1.0f )
	{
		_transformation.setIdentity();
	}

	inline GLSceneView::~GLSceneView()
	{
	}

	/*inline GLScene* GLSceneView::scene()
	{
		return _scene;
	}*/

	inline void GLSceneView::paintGLEvent( PaintEvent& )
	{
		setViewport( rect() );
		_prog.bind();

		{
			Matrix4f proj, tmp;
			GL::perspective( proj, 60.0f, ( float ) rect().width / ( float ) rect().height, 0.01f, 100.0f );
			tmp.setIdentity();
			tmp[ 2 ][ 3 ] = -2.5f;
			Matrix4f diag;
			diag.setDiagonal( Vector4f( _scale, _scale, _scale, 1.0f ) );
			tmp *= diag * _transformation;

			_prog.setProjection( proj * tmp );

			glEnable( GL_DEPTH_TEST );
                        glPointSize( 3.0f );
                        _points.draw();
			glDisable( GL_DEPTH_TEST );

		}

		_prog.unbind();
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

        inline void GLSceneView::setScenePoints( const ScenePoints& pts )
        {
            _points.setScenePoints( pts );
            update();
        }
}

#endif
