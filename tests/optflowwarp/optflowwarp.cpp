#include <iostream>
#include <cv.h>
#include <highgui.h>

#include <cvt/gfx/Image.h>
#include <cvt/io/ImageIO.h>
#include <cvt/gfx/ifilter/ROFDenoise.h>
#include <cvt/util/Exception.h>
#include <cvt/io/FloFile.h>
#include <cvt/vision/Flow.h>

#include "calcflow.h"

using namespace cvt;

int main(int argc, char* argv[])
{
	int key;
	Image img1,img2;
	Image in1, in2;
	Image flow;
	Image _gt;
	Image* gt = NULL;

	if( argc < 3 )
		return 1;
	try {
		ImageIO::loadPNG( img1, argv[ 1 ] );
		ImageIO::loadPNG( img2, argv[ 2 ] );

		in1.reallocate( img1.width(), img1.height(), IOrder::GRAY, IType::FLOAT );
		in2.reallocate( img2.width(), img2.height(), IOrder::GRAY, IType::FLOAT );
		img1.convert( in1, IOrder::GRAY, IType::FLOAT );
		img2.convert( in2, IOrder::GRAY, IType::FLOAT );

#if 0
		ROFDenoise rof;
		Image* itmp = new Image();
		itmp->reallocate( img1.width(), img1.height(), IOrder::GRAY, IType::FLOAT );
		rof.apply( *itmp, in1, 0.1f, 25 );
		in1.mad( *itmp, -0.5f );

		rof.apply( *itmp, in2, 0.1f, 25 );
		in2.mad( *itmp, -0.5f );

		delete itmp;
#endif

		if( argc == 4 ) {
			FloFile::FloReadFile( _gt, argv[ 3 ] );
			gt = &_gt;
		}

		calcflow( flow, in1, in2, gt );

		FloFile::FloWriteFile( flow, "out.flo" );
		sleep( 1 );
	} catch( Exception e ) {
		std::cout << e.what() << std::endl;
	}
	return 0;
}
