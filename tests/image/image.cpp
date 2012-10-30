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
	String inputFile = resources.find( "lena.png" );
	String inputGray = resources.find( "lena_g.png" );
	String inputSTest = resources.find( "scaletest.png" );

	try {
		{
			Image ppm( resources.find( "lena.ppm" ) );
			ppm.save( "ppm.png" );
			ppm.load( resources.find( "ppmtestc.ppm" ) );
			ppm.save( "ppmc.png" );


			cvt::Image stst;
			stst.load( inputSTest );
			cvt::Image stst2( stst.width(), stst.height(), cvt::IFormat::floatEquivalent( stst.format() ) );
			stst.convert( stst2 );
			cvt::IScaleFilterBilinear filter;
			cvt::Image out;
			stst2.scale( out, stst.width() / 2, stst.height( ) / 2, filter );
			out.save( "scaleoutput.png" );
		}

		// RGBA UBYTE IMAGE
		cvt::Image img, img2, gradx;
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

		gradx.reallocate( imgGray.width(), imgGray.height(), IFormat::GRAY_INT16 );
		std::cout << "Loaded image: " << imgGray << std::endl;
		std::cout << "Dest. image: " << gradx << std::endl;
		imgGray.convolve( gradx, IKernel::HAAR_HORIZONTAL_3, IKernel::GAUSS_VERTICAL_3 );

		gradx.save( "i16_grad.png" );

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
