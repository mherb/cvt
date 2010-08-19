#include <iostream>

#include <cvt/gfx/Image.h>
#include <cvt/io/ImageIO.h>
#include <cvt/gfx/Color.h>

#include <cv.h>
#include <highgui.h>

#include <string>

int main(int argc, char* argv[])
{
    std::string dataFolder(DATA_FOLDER);
    std::string inputFile(dataFolder + "/atomium.png");
    std::string inputGray(dataFolder + "/sebi_wa.png");
        
    // RGBA UBYTE IMAGE
    cvt::Image img(640, 480);
    cvt::Image imgGray(640, 480, cvt::CVT_GRAYALPHA, cvt::CVT_UBYTE);
    
    cvt::ImageIO::loadPNG(img, inputFile);
        
    cvNamedWindow("Test Image");
    
    cvShowImage("Test Image", img.iplimage());
    cvWaitKey(0);    
    cvt::ImageIO::savePNG(img, "out.png");
    
    cvt::ImageIO::loadPNG(imgGray, inputGray);
    
    if(imgGray.order() == cvt::CVT_GRAY)
	std::cout << "Loaded grayscale image" << std::endl;
    
    if(imgGray.order() == cvt::CVT_GRAYALPHA)
	std::cout << "Loaded grayscale image with alpha channel" << std::endl;
    
    cvt::ImageIO::savePNG(imgGray, "out_gray.png");
        
    return 0;    
}