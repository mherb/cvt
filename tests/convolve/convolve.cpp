#include <iostream>

#include <cvt/gfx/Image.h>
#include <cvt/gfx/IKernel.h>
#include <cvt/gfx/ifilter/ROFDenoise.h>
#include <cvt/io/ImageIO.h>
#include <cvt/gfx/Color.h>
#include <cvt/util/Exception.h>

#include <string>

int main(int argc, char* argv[])
{
	std::string dataFolder(DATA_FOLDER);
	std::string inputFile(dataFolder + "/lena.png");
	
	try {
		// RGBA UBYTE IMAGE
		cvt::Image img;
		cvt::Image out;
		cvt::ImageIO::loadPNG(img, inputFile);
		
		out.reallocate( img );

		cvt::IKernel kern( 3, 3 );
		for( int x = 0; x < 3; x++ )
			for( int y = 0; y < 3; y++ )
				kern( x, y ) = 1.0f / 9.0f;
		img.convolve( out, kern );
		cvt::ImageIO::savePNG(out, "convolve1.png");

		img.convolve( out, cvt::IKernel::MEAN_HORIZONTAL_3, cvt::IKernel::MEAN_VERTICAL_3);
		cvt::ImageIO::savePNG(out, "convolve2.png");
	
		
	} catch( cvt::Exception e ) {
		std::cerr << e.what() << std::endl;
		return 1;
	}

	return 0;
}
