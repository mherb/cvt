#include <iostream>
#include <cvt/gui/Application.h>
#include <cvt/gui/Window.h>
#include <cvt/gui/WidgetLayout.h>
#include <cvt/gui/Moveable.h>
#include <cvt/gui/Button.h>
#include <cvt/gui/ImageView.h>
#include <cvt/gui/BasicTimer.h>
#include <cvt/io/ImageIO.h>
#include <cvt/io/Resources.h>
#include <cvt/util/Time.h>
#include <cvt/gui/Slider.h>

#include <cvt/gl/GLVertexArray.h>

#include <cvt/gl/shader/150/basiclight_150_vert.h>
#include <cvt/gl/shader/150/basiclight_150_frag.h>
#include <cvt/gl/shader/120/basiclight_120_vert.h>
#include <cvt/gl/shader/120/basiclight_120_frag.h>

using namespace cvt;

void timeout( BasicTimer* t )
{
//	std::cout << "Timeout" << std::endl;
//	usleep( 50000 );
}

int main(int argc, char* argv[])
{
    cvt::Resources resources;
	Image img;
	ImageIO::loadPNG( img, resources.find( "lena.png" ) );

	cvt::Window w( "Test" );
    w.setSize( 640, 480 );
    w.setVisible( true );
    w.setMinimumSize( 320, 240 );

    Button button( "Quit" );
	Delegate<void ()> dquit( &Application::exit );
	button.clicked.add( &dquit );

	Slider<float> slider( 0, 1.0f, 0.5f );
    WidgetLayout wlslider;
    wlslider.setAnchoredRight( 70, 100 );
    wlslider.setAnchoredBottom( 14, 12 );
    w.addWidget( &slider, wlslider );

    WidgetLayout wl;
    wl.setAnchoredRight( 10, 50 );
    wl.setAnchoredBottom( 10, 20 );
    w.addWidget( &button, wl );

	ImageView view;
	Moveable m( &view );
	m.setSize( 200, 200 );
	w.addWidget( &m );

	view.setImage( img );

	ImageView view2;
	Moveable m2( &view2 );
	m2.setSize( 200, 200 );
	m2.setPosition( 200, 0 );
	w.addWidget( &m2 );
	view2.setImage( img );

	BasicTimer t( 0 );
	Delegate<void (BasicTimer*)> d( timeout );
	t.timeout.add( &d );
	t.start();

    Application::run();
    return 0;
}
