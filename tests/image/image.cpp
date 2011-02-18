#include <iostream>

#include <cvt/gfx/Image.h>
#include <cvt/gfx/ifilter/ROFDenoise.h>

#include <cvt/io/ImageIO.h>
#include <cvt/io/Resources.h>

#include <cvt/gfx/Color.h>
#include <cvt/util/Exception.h>

#include <string>

using namespace cvt;

int main()
{
	std::string dataFolder(DATA_FOLDER);
	cvt::Resources resources;
	std::string inputFile = resources.find( "lena.png" );
	std::string inputGray = resources.find( "lena_g.png" );
	
	try {
		// RGBA UBYTE IMAGE
		cvt::Image img, img2;		
		cvt::ImageIO::loadPNG(img, inputFile );
		cvt::Image imgF( img.width(), img.height(), cvt::IFormat::floatEquivalent( img.format() ) );		
		img.convert( imgF );

		cvt::ImageIO::savePNG( imgF, "float.png" );
		
		cvt::Image resized;
		
		//cvt::IScaleFilterBilinear filter;
		//cvt::IScaleFilterCubic filter;
		//cvt::IScaleFilterLanczos filter;
		cvt::IScaleFilterMitchell filter;
		img.scale( resized, 1024, 1024, filter );
		resized.scale( img2, 512, 512, filter );
		
		std::cout << "SAD: " << img.sad( img2 ) / cvt::Math::sqr( 512.0f )  << std::endl;
		
		cvt::ImageIO::savePNG( img2, "rescaled.png" );
		
		cvt::Image imgGray(img.width(), 
						   img.height(),
						   cvt::IFormat::GRAYALPHA_UINT8 );
		
		cvt::ImageIO::savePNG(img, "out.png");
		
		cvt::ImageIO::loadPNG(imgGray, inputGray);
		
		std::cout << "Loaded image: " << imgGray << std::endl;
		
		// save the gray image
        cvt::ImageIO::saveRAW( imgGray, "gray.cvtraw" );
        cvt::ImageIO::loadRAW( imgGray, "gray.cvtraw" );
        
		cvt::ImageIO::savePNG(imgGray, "out_gray.png");
		
	} catch( cvt::Exception e ) {
		std::cerr << e.what() << std::endl;
		return 1;
	}

	return 0;
}
