#include <iostream>

#include <cvt/gfx/Image.h>
#include <cvt/util/Exception.h>

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
		
		
		cvNamedWindow( "Test Image" );
		
		cvShowImage( "Test Image", imgGray.iplimage() );
		cvWaitKey( 0 );
		
		cvt::Image gray;
		imgGray.convert( gray, cvt::CVT_GRAY, cvt::CVT_FLOAT );		
				
		
	} catch( cvt::Exception e ) {
		std::cerr << e.what() << std::endl;
		return 1;
	}
}