#include <cvt/gfx/Image.h>
#include <cvt/gui/ImageView.h>
#include <cvt/gui/Window.h>
#include <cvt/gui/WidgetLayout.h>
#include <cvt/gui/Application.h>

using namespace cvt;

int main( int argc, char* argv[] )
{
	if( argc < 2 ){
		std::cout << "Usage: " << argv[ 0 ] << " <imagefile>" << std::endl;
		return 0;
	}

	Image img( argv[ 1 ] );

    std::cout << argv[ 1 ] << ": " << img << std::endl;

	Window w( "imageView" );
	w.setSize( 320, 240 );
	ImageView imview;
	imview.setSize( 320, 240 );
	imview.setImage( img );

	WidgetLayout wl;
	wl.setAnchoredTopBottom( 0, 0 );
	wl.setAnchoredLeftRight( 0, 0 );
	w.addWidget( &imview, wl );
	w.setVisible( true );

	w.update();

	Application::run();

	return 0;
}
