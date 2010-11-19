#include <iostream>

#include <cvt/gfx/Image.h>
#include <cvt/gfx/ifilter/ROFDenoise.h>
#include <cvt/io/ImageIO.h>
#include <cvt/io/Resources.h>

#include <cvt/gfx/Color.h>
#include <cvt/util/Exception.h>


#include <cv.h>
#include <highgui.h>

#include <string>

int main(int argc, char* argv[])
{
	std::string dataFolder(DATA_FOLDER);
	cvt::Resources resources;
	std::string inputFile = resources.find( "lena.png" );
	std::string inputGray = resources.find( "lena_ga.png" );
	
	try {
		// RGBA UBYTE IMAGE
		cvt::Image img;		
		cvt::ImageIO::loadPNG(img, inputFile);
		cvt::Image imgGray(img.width(), 
						   img.height(),
						   cvt::IFormat::GRAYALPHA_UINT8 );
		
		cvNamedWindow("Test Image");
		
//		cvShowImage("Test Image", img.iplimage());
		cvWaitKey(0);
		
		cvt::ImageIO::savePNG(img, "out.png");
		
		cvt::ImageIO::loadPNG(imgGray, inputGray);
		
		std::cout << "Loaded image: " << imgGray << std::endl;
		
		// save the gray image
		cvt::ImageIO::savePNG(imgGray, "out_gray.png");
		
		
	} catch( cvt::Exception e ) {
		std::cerr << e.what() << std::endl;
		return 1;
	}

	return 0;
}
