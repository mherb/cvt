#include <iostream>

#include <cvt/gfx/Image.h>
#include <cvt/util/Exception.h>
#include <cvt/geom/Rect.h>
#include <cvt/io/ImageIO.h>

int main()
{
	std::string dataFolder( DATA_FOLDER );
	std::string inputFile( dataFolder + "/lena_g.png" );
	
	try {
		// RGBA UBYTE IMAGE
		cvt::Image imgGray;		
		cvt::ImageIO::loadPNG( imgGray, inputFile );
		
		cvt::Image imgF( imgGray.width(), imgGray.height(), cvt::IFormat::floatEquivalent( imgGray.format() ) );
		imgGray.convert( imgF );
		
		cvt::Recti rect(100, 100, 100, 100);
		cvt::Image patchA(imgF, &rect, false);
		
		//rect.x = 100;
		cvt::Image patchB(imgF, &rect, false);		
		
		std::cout << "SSD: " << patchA.ssd( patchB ) << std::endl;
		std::cout << "SAD: " << patchA.sad( patchB ) << std::endl;
		
	} catch( cvt::Exception e ) {
		std::cerr << e.what() << std::endl;
		return 1;
	}
}
