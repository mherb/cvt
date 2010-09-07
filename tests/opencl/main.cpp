#include <cvt/cl/CLContext.h>
#include <cvt/cl/CLKernel.h>
#include <cvt/cl/CLImage.h>
#include <cvt/gfx/Image.h>
#include <cvt/gfx/Color.h>
#include <cvt/io/ImageIO.h>
#include <cvt/util/Exception.h>
#include <string>
#include <iostream>

#include "simple.h"

using namespace cvt;

int main()
{
	try {
		CLContext cl;
		cl.makeCurrent();

		CLKernel kernel;

		//	cl.info( std::cout );
		//	cl.infoImageFormats( std::cout );

		std::string log;
		kernel.build( "simple", _simple_source, strlen( _simple_source ), log);
		std::cout << "Log: " << log << std::endl;

		CLImage image( 640, 480 );
		Color c( 1.0f, 0.0f, 0.0f, 1.0f );
		kernel.setArg( 0 , &image );
		kernel.setArg( 1 , &c );
		kernel.run( ::cl::NullRange, image.globalRange(), ::cl::NDRange( 16, 16 ) );
		Image i;
		image.readImage( i );
		ImageIO::savePNG( i, "out.png" );
	} catch( Exception e ) {
		std::cout << e.what() << std::endl;
	}
}
