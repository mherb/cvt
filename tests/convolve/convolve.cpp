#include <iostream>

#include <cvt/gfx/Image.h>
#include <cvt/gfx/IKernel.h>
#include <cvt/gfx/ifilter/ROFDenoise.h>
#include <cvt/gfx/Color.h>
#include <cvt/util/Exception.h>

#include <string>

int main(int argc, char* argv[])
{
	std::string dataFolder(DATA_FOLDER);
	std::string inputFile(dataFolder + "/lena.png");
	std::string inputFileg(dataFolder + "/lena_g.png");
	
	try {
		// RGBA UBYTE IMAGE
		cvt::Image img;
		cvt::Image out;
        img.load( inputFile.c_str() );

		out.reallocate( img );
		out.fill( cvt::Color( 0.0f,0.0f,0.0f,1.0f) );

		cvt::IKernel kern( 3, 3 );
		for( int x = 0; x < 3; x++ )
			for( int y = 0; y < 3; y++ )
				kern( x, y ) = 1.0f / 9.0f;
		img.convolve( out, kern );
		out.save( "convolve1.png");

		out.fill( cvt::Color( 0.0f,0.0f,0.0f,1.0f) );
		img.convolve( out, cvt::IKernel::MEAN_HORIZONTAL_3, cvt::IKernel::MEAN_VERTICAL_3);
		out.save( "convolve2.png");

		img.load( inputFileg.c_str() );

		out.reallocate( img );
		out.fill( cvt::Color( 0.0f,0.0f,0.0f,1.0f) );

		for( int x = 0; x < 3; x++ )
			for( int y = 0; y < 3; y++ )
				kern( x, y ) = 1.0f / 9.0f;
		img.convolve( out, kern );
		out.save( "convolveg1.png");

		out.fill( cvt::Color( 0.0f,0.0f,0.0f,1.0f) );
		img.convolve( out, cvt::IKernel::MEAN_HORIZONTAL_3, cvt::IKernel::MEAN_VERTICAL_3);
		out.save( "convolveg2.png");

		cvt::Image imgf( img.width(), img.height(), cvt::IFormat::GRAY_FLOAT );
		img.convert( imgf );
		out.reallocate( imgf );
		out.fill( cvt::Color( 0.0f,0.0f,0.0f,1.0f) );
		imgf.convolve( out, cvt::IKernel::MEAN_HORIZONTAL_3, cvt::IKernel::MEAN_VERTICAL_3);
		out.save( "convolveg2-float.png");
		
	} catch( cvt::Exception e ) {
		std::cerr << e.what() << std::endl;
		return 1;
	}

	return 0;
}
