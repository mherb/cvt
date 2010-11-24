#include <iostream>
#include <cvt/gui/Application.h>
#include <cvt/gui/Window.h>
#include <cvt/gui/WidgetLayout.h>
#include <cvt/gui/Moveable.h>
#include <cvt/gui/Button.h>
#include <cvt/gui/BasicTimer.h>
#include <cvt/util/Time.h>

using namespace cvt;

void timeout( BasicTimer* timer )
{
    static Time t;

    std::cout << "Timeout: " << t.elapsedMilliSeconds() << " ms" << std::endl;
    t.reset();
}

int main(int argc, char* argv[])
{
//    BasicTimer t( 30 );

    Window w( "Test" );
    w.setSize( 640, 480 );
    w.setVisible( true );
    w.setMinimumSize( 320, 240 );

    Button button( "Quit" );
    WidgetLayout wl;
    wl.setAnchoredRight( 10, 50 );
    wl.setAnchoredBottom( 10, 20 );
    w.addWidget( &button, wl );

	Moveable m( NULL );
	m.setSize( 200, 200 );
	w.addWidget( &m );

  //  Delegate<void (BasicTimer*)> d( &timeout );
    //t.timeout.add( &d );
    //t.start();

    Application::run();
    return 0;
}
