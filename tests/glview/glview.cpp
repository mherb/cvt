#include <cvt/gui/Application.h>
#include <cvt/gui/Window.h>
#include <cvt/gui/Moveable.h>
#include <cvt/gui/GLView.h>

#include <cvt/gl/GLTeapot.h>
#include <cvt/gl/progs/GLBasicProg.h>

using namespace cvt;

class MyGLView : public GLView
{
	public:
		MyGLView() : 
			_near( 0.1f ),
			_far( 1000.0f )
		{
		}

		void paintGLEvent( PaintEvent* )
		{
			Recti r = rect();
			float asp = ( float )r.width / ( float )r.height;
			setViewport( 0, 0, r.width, r.height );

			Matrix4f persp, trans;	
			GL::perspective( persp, 60.0f, asp, _near, _far );

			trans.setIdentity();
			trans[ 2 ][ 3 ] = -20.0f;

			persp *= trans;

			_basicProg.bind();

			_basicProg.setProjection( persp );

			_teapot.draw();

			_basicProg.unbind();

		}

		void addPoint( const Vector3f & p )
		{
			// alloc neccessary space in vao and add a point
		}

	private:
		// view transform (virtual cam)
		Matrix4f	_viewT;
		float		_near;
		float		_far;

		GLBasicProg	_basicProg;
		GLTeapot	_teapot;

		// base level grid
		GLVertexArray	_grid;

		void createGrid()
		{
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
