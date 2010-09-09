#include <iostream>

#include <cvt/gfx/Image.h>
#include <cvt/gfx/ifilter/Sobel.h>
#include <cvt/io/ImageIO.h>

#include <cvt/util/Exception.h>


#include <cv.h>
#include <highgui.h>

#include <string>

int main( int argc, char* argv[] )
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
		
		cvt::Image sobelX( imgGray.width(), 
						  imgGray.height(), 
						  cvt::CVT_GRAY, 
						  cvt::CVT_FLOAT );
		
		cvt::Image sobelY( imgGray.width(), 
						  imgGray.height(), 
						  cvt::CVT_GRAY, 
						  cvt::CVT_FLOAT );
		
		cvt::Image sobelMag( imgGray.width(), 
						     imgGray.height(), 
						     cvt::CVT_GRAY, 
						     cvt::CVT_FLOAT );
		cvt::Image suppr(imgGray.width(), 
						 imgGray.height(), 
						 cvt::CVT_GRAY, 
						 cvt::CVT_FLOAT );
		
		cvt::Image gray;
		imgGray.convert( gray, cvt::CVT_GRAY, cvt::CVT_FLOAT );
		
		cvt::Sobel sobel;
		
		sobel.apply( sobelX, sobelY, gray, true );
		cvt::Sobel::magnitude( sobelMag, sobelX, sobelY );
		cvt::Sobel::nonMaximalSuppression(suppr, sobelX, sobelY, sobelMag);
		
		sobelX.convert( imgGray, cvt::CVT_GRAY, cvt::CVT_UBYTE );
		cvShowImage( "Test Image", imgGray.iplimage() );
		cvWaitKey( 0 );		
		
		sobelY.convert( imgGray, cvt::CVT_GRAY, cvt::CVT_UBYTE );
		cvShowImage( "Test Image", imgGray.iplimage() );
		cvWaitKey( 0 );		
		
		sobelMag.convert( imgGray, cvt::CVT_GRAY, cvt::CVT_UBYTE );
		cvShowImage( "Test Image", imgGray.iplimage() );
		cvWaitKey( 0 );
		
		suppr.convert( imgGray, cvt::CVT_GRAY, cvt::CVT_UBYTE );
		cvShowImage( "Test Image", imgGray.iplimage() );
		cvWaitKey( 0 );
				
	} catch( cvt::Exception e ) {
		std::cerr << e.what() << std::endl;
		return 1;
	}

	return 0;
}
