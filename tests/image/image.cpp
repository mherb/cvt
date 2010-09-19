#include <iostream>

#include <cvt/gfx/Image.h>
#include <cvt/gfx/ifilter/ROFDenoise.h>
#include <cvt/io/ImageIO.h>
#include <cvt/gfx/Color.h>
#include <cvt/util/Exception.h>


#include <cv.h>
#include <highgui.h>

#include <string>

int main(int argc, char* argv[])
{
	std::string dataFolder(DATA_FOLDER);
	std::string inputFile(dataFolder + "/lena.png");
	std::string inputGray(dataFolder + "/lena_ga.png");
	
	try {
		// RGBA UBYTE IMAGE
		cvt::Image img;		
		cvt::ImageIO::loadPNG(img, inputFile);
		cvt::Image imgGray(img.width(), 
						   img.height(), 
						   cvt::IOrder::GRAYALPHA, 
						   cvt::IType::UBYTE);
		
		cvNamedWindow("Test Image");
		
		cvShowImage("Test Image", img.iplimage());
		cvWaitKey(0);
		
		cvt::ImageIO::savePNG(img, "out.png");
		
		cvt::ImageIO::loadPNG(imgGray, inputGray);
		
		if(imgGray.order() == cvt::IOrder::GRAY )
			std::cout << "Loaded grayscale image" << std::endl;
		else if(imgGray.order() == cvt::IOrder::GRAYALPHA )
			std::cout << "Loaded grayscale image with alpha channel" << std::endl;
		
		// save the gray image
		cvt::ImageIO::savePNG(imgGray, "out_gray.png");
		
		
	} catch( cvt::Exception e ) {
		std::cerr << e.what() << std::endl;
		return 1;
	}

	return 0;
}
