#include <iostream>

#include <cvt/gfx/Image.h>
#include <cvt/util/Exception.h>
#include <cvt/util/Rect.h>
#include <cvt/io/ImageIO.h>

#include <cv.h>
#include <highgui.h>

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
		
		cvNamedWindow( "Test Image" );
		
		cvt::Recti rect(100, 100, 100, 100);
		cvt::Image patchA(imgF, &rect, false);
		
		//rect.x = 100;
		cvt::Image patchB(imgF, &rect, false);		
		
		std::cout << "SSD: " << patchA.ssd( patchB ) << std::endl;
		std::cout << "SAD: " << patchA.sad( patchB ) << std::endl;
		
//		cvShowImage( "Test Image", patchA.iplimage() );
		cvWaitKey( 0 );
//		cvShowImage( "Test Image", patchB.iplimage() );
		cvWaitKey( 0 );
		
//		cvShowImage( "Test Image", imgGray.iplimage() );
		cvWaitKey( 0 );
	} catch( cvt::Exception e ) {
		std::cerr << e.what() << std::endl;
		return 1;
	}
}
