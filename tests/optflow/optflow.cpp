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
	Image flow, cflow;
	ROFDenoise denoise;
	Image _gt;
	Image* gt = NULL;

	if( argc < 3 )
		return 1;

	try {
	ImageIO::loadPNG( img1, argv[ 1 ] );
	ImageIO::loadPNG( img2, argv[ 2 ] );

//#define SCALE 2

	Image* tmp = new Image();
#if 1
	img1.convert( *tmp, IOrder::BGRA, IType::FLOAT );
#ifdef SCALE
	{
		Image x;
		IScaleFilterBilinear sf;
		tmp->scale( x, tmp->width()/SCALE, tmp->height()/SCALE, sf );
		tmp->copy( x );
	}
#endif
	denoise.apply( in1, *tmp, 0.1f, 100 );
	in1.mad( *tmp, -0.95f );
	in1.mul( 5.0f );
	img2.convert( *tmp, IOrder::BGRA, IType::FLOAT );
#if SCALE
	{
		Image x;
		IScaleFilterBilinear sf;
		tmp->scale( x, tmp->width()/SCALE, tmp->height()/SCALE, sf );
		tmp->copy( x );
	}
#endif
	denoise.apply( in2, *tmp, 0.1f, 100 );
	in2.mad( *tmp, -0.95f );
	in2.mul( 5.0f );
#else
	img1.convert( in1, IOrder::BGRA, IType::FLOAT );
	img2.convert( in2, IOrder::BGRA, IType::FLOAT );
#endif
	delete tmp;

	cvShowImage( "Frame 1", in1.iplimage() );
	cvShowImage( "Frame 2", in2.iplimage() );
	if( argc == 4 ) {
		FloFile::FloReadFile( _gt, argv[ 3 ] );
#ifdef SCALE
		{
			Image x;
			IScaleFilterBilinear sf;
			_gt.scale( x, _gt.width()/SCALE, _gt.height()/SCALE, sf );
			_gt.copy( x );
			_gt.mul( 1.0f / ( float ) SCALE );
		}
#endif
		gt = &_gt;
	}

	calcflow( flow, in1, in2, gt );
//	Flow::colorCode( cflow, flow );

/*	while( 1 ) {
		cvShowImage( "Flow", cflow.iplimage() );
		key = cvWaitKey( 10 ) & 0xff;
		if( key == 27 )
			break;
	}*/

	FloFile::FloWriteFile( flow, "out.flo" );
	} catch( Exception e ) {
		std::cout << e.what() << std::endl;
	}
	return 0;
}
