#include <cvt/gui/Application.h>
#include <cvt/gui/Window.h>
#include <cvt/gui/Moveable.h>
#include <cvt/gui/GLView.h>

#include <cvt/gl/GLTeapot.h>
#include <cvt/geom/ArcBall.h>
#include <cvt/gl/progs/GLBasicProg.h>

using namespace cvt;

class MyGLView : public GLView
{
	public:
		MyGLView() : 
			_trans( -10 ),
			_near( 0.1f ),
			_far( 100.0f )
		{
			createGrid( 10 );
			_rot.setIdentity();
		}

	protected:

		void paintGLEvent( PaintEvent* )
		{
			Recti r = rect();
			float asp = ( float )r.width / ( float )r.height;
			setViewport( 0, 0, r.width, r.height );

			Matrix4f persp;	
			GL::perspective( persp, 60.0f, asp, _near, _far );

			Matrix4f view;
			view.setIdentity();
			view[ 2 ][ 3 ] = _trans;

			persp = persp * view * _rot;

			_basicProg.bind();

			_basicProg.setProjection( persp );

			glLineWidth( 1.0f );
			_grid.draw( GL_LINES, 0, _numLines );

			_teapot.draw();

			_basicProg.unbind();
		}


		void mousePressEvent( MousePressEvent* e )
		{
			if( e->button() == 1 ) {
				_press.x = e->x;
				_press.y = e->y;
			}
		}


		void mouseReleaseEvent( MouseReleaseEvent* e )
		{
			if( e->button() == 4 ) {
				_trans += 0.25f;
				update();
			} else if( e->button() == 5 ) {
				_trans -= 0.25f;
				update();
			}
		}

		void mouseMoveEvent( MouseMoveEvent* e )
		{
			if( e->buttonMask() & 1 ) {
				Matrix4f rot;
				_arcball.getRotation( rot, _press.x, _press.y, e->x, e->y );
				
				_rot = rot * _rot;

				update();
				_press.x = e->x;
				_press.y = e->y;
			}
		}


		void resizeEvent( ResizeEvent* e )
		{
			_arcball.setViewportSize( e->width(), e->height() );
		}

		void updateCameraPose( const Matrix4f & m )
		{

		}

	private:
		// view transform (virtual cam)
		Matrix4f	_rot;
		float		_trans;
		float		_near;
		float		_far;

		ArcBall		_arcball;
		Vector2i	_press;

		GLBasicProg	_basicProg;
		GLTeapot	_teapot;

		// base level grid
		GLVertexArray	_grid;
		GLBuffer		_gridLines;
		size_t			_numLines;

		void createGrid( size_t halfRes )
		{
			_gridLines.alloc( GL_STATIC_DRAW, sizeof( GLfloat ) * 3 * 2 * 2 * ( 2 * halfRes + 1 ), NULL );

			_numLines = 2 * 2 * ( 2 * halfRes + 1 );

			GLfloat * lineData = ( GLfloat* )_gridLines.map();
			for( float x = -halfRes; x <= halfRes; x+=1.0f  ){
				lineData[ 0 ] = x;
				lineData[ 1 ] = 0.0f;
				lineData[ 2 ] = -halfRes;
				lineData[ 3 ] = x;
				lineData[ 4 ] = 0.0f;
				lineData[ 5 ] = halfRes;

				lineData[ 6  ] = -halfRes;
				lineData[ 7  ] = 0;
				lineData[ 8  ] = x;
				lineData[ 9  ] = halfRes;
				lineData[ 10 ] = 0;
				lineData[ 11 ] = x;
				lineData += 12;
			}

			_gridLines.unmap();

			_grid.setVertexData( _gridLines, 3, GL_FLOAT );
			_grid.setColor( Color::WHITE );
		}

};

int main( int argc, char* argv[] )
{
	Window w( "Main" );

	w.setSize( 640, 480 );
	MyGLView mgl;

	Moveable m( &mgl );
	m.setSize( 100, 100 );
	m.setPosition( 120, 120 );
	m.setVisible( true );
	w.addWidget( &m );


	w.setVisible( true );

	Application::run();

	return 0;
}
