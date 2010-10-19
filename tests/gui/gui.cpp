#include <iostream>
#include <cvt/gui/Application.h>
#include <cvt/gui/Window.h>

using namespace cvt;

int main(int argc, char* argv[])
{
    Window w( "Test" );
	w.setSize( 640, 480 );
	w.setVisible( true );

    Application::run();
	return 0;
}
