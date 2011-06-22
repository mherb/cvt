#include <iostream>

#include <cvt/gfx/Image.h>
#include <cvt/gfx/ifilter/ROFDenoise.h>

#include <cvt/io/Resources.h>

#include <cvt/gfx/Color.h>
#include <cvt/util/Exception.h>

#include <string>

using namespace cvt;

int main()
{
	cvt::Resources resources;
	std::string inputFile = resources.find( "lena.png" );
	std::string inputGray = resources.find( "lena_g.png" );
	
	try {
		// RGBA UBYTE IMAGE
		cvt::Image img, img2;		
		img.load( inputFile.c_str() );
		cvt::Image imgF( img.width(), img.height(), cvt::IFormat::floatEquivalent( img.format() ) );		
		img.convert( imgF );

		imgF.save( "float.png" );
		
		cvt::Image resized;
		
		//cvt::IScaleFilterBilinear filter;
		//cvt::IScaleFilterCubic filter;
		//cvt::IScaleFilterLanczos filter;
		cvt::IScaleFilterMitchell filter;
		img.scale( resized, 1024, 1024, filter );
		resized.scale( img2, 512, 512, filter );
		
		std::cout << "SAD: " << img.sad( img2 ) / cvt::Math::sqr( 512.0f )  << std::endl;
		
		img2.save( "rescaled.png" );
		
		cvt::Image imgGray(img.width(), 
						   img.height(),
						   cvt::IFormat::GRAYALPHA_UINT8 );
		
		img.save( "out.png");
		
		imgGray.load( inputGray.c_str() );
		
		std::cout << "Loaded image: " << imgGray << std::endl;
		
		// save the gray image
        imgGray.save( "gray.cvtraw" );
        imgGray.load( "gray.cvtraw" );
        
		imgGray.save( "out_gray.png");
		
	} catch( cvt::Exception e ) {
		std::cerr << e.what() << std::endl;
		return 1;
	}

	return 0;
}
