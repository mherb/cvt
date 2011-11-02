#include <cvt/gui/Application.h>
#include <cvt/gui/Window.h>
#include <cvt/gui/Moveable.h>
#include <cvt/gui/GLView.h>

using namespace cvt;

class MyGLView : public GLView
{
	public:
		MyGLView(){}

		void paintGLEvent( PaintEvent* )
		{
			Recti r = rect();
			glViewport( r.x, r.y, r.width, r.height );

			glClearColor( 1.0f, 0.0f, 0.0f, 1.0f );
			glClear( GL_COLOR_BUFFER_BIT );
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
