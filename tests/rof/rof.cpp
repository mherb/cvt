#include <iostream>

#include <cvt/gfx/Image.h>
#include <cvt/gfx/IKernel.h>
#include <cvt/gfx/ifilter/ROFDenoise.h>
#include <cvt/gfx/Color.h>
#include <cvt/util/Exception.h>

#include <string>

int main()
{
	std::string dataFolder(DATA_FOLDER);
	std::string inputFile(dataFolder + "/lena.png");
	
	try {
		// RGBA UBYTE IMAGE
		cvt::Image img;
		cvt::Image out, tmp;
		img.load( inputFile.c_str() );
		
		out.reallocate( img.width(), img.height(), cvt::IFormat::floatEquivalent( img.format() ) );
		tmp.reallocate( out );
		img.convert( tmp );

		cvt::ROFDenoise rof;
		rof.apply( out, tmp, 0.05f, 5 );

		out.save( "out.png" );

		cvt::Image x( tmp );
		x.sub( out );
		x.mul( 1.5f );
		out.add( x );
		out.save( "out2.png" );
		
	} catch( cvt::Exception e ) {
		std::cerr << e.what() << std::endl;
		return 1;
	}

	return 0;
}
