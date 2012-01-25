#include <cvt/gfx/Image.h>
#include <cvt/gui/Application.h>

#include "KLTWindow.h"

using namespace cvt;

int main( int argc, char* argv[] )
{
	if( argc < 2 ){
		std::cout << "Usage: " << argv[ 0 ] << " <image>" << std::endl;
		return 0;
	} 

	Image img( argv[ 1 ] );
	KLTWindow kltWin( img );
	Application::run();

	return 0;
}
