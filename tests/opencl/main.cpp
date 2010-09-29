#include <cvt/cl/CLContext.h>
#include "CLOptflow.h"
#include <cvt/gfx/Image.h>
#include <cvt/gfx/Color.h>
#include <cvt/gfx/IFilterScalar.h>
#include <cvt/io/ImageIO.h>
#include <cvt/util/Exception.h>
#include <cvt/util/Timer.h>
#include <cvt/io/FloFile.h>
#include <cvt/vision/Flow.h>
#include <string>
#include <iostream>
#include <cv.h>
#include <highgui.h>
#include <cvt/io/V4L2Camera.h>

#include "calcp1.h"
#include "calcp2.h"
#include "clearimg.h"
#include "copyimg.h"
#include "bilindown.h"
#include "warp.h"
#include "warpsub.h"
#include "grad.h"

using namespace cvt;

int main( int argc, char** argv )
{
	std::string log;
	int key;
	std::vector<cl::Event> sync;
	cl::Event event;
	int pyridx = 0;
	CLImage* u;
	Image img1, img2, in1, in2, inp1, inp2, iflow, flowpad;


	try {
		CLContext cl;
		cl.makeCurrent();
		CLOptflow flow;

		ImageIO::loadPNG( img1, argv[ 1 ] );
		ImageIO::loadPNG( img2, argv[ 2 ] );

		in1.reallocate( img1.width(), img1.height(), IOrder::GRAY, IType::FLOAT );
		in2.reallocate( img2.width(), img2.height(), IOrder::GRAY, IType::FLOAT );
		iflow.reallocate( img2.width(), img2.height(), IOrder::GRAYALPHA, IType::FLOAT );
		flowpad.reallocate( 640, 480, IOrder::GRAYALPHA, IType::FLOAT );
		inp1.reallocate( 640, 480, IOrder::GRAY, IType::FLOAT );
		inp2.reallocate( 640, 480, IOrder::GRAY, IType::FLOAT );
		img1.convert( in1, IOrder::GRAY, IType::FLOAT );
		img2.convert( in2, IOrder::GRAY, IType::FLOAT );
		inp1.fill( Color( 0.0f ) );
		inp2.fill( Color( 0.0f ) );
		inp1.copyRect( 0,0, in1, 0, 0, ( int ) in1.width(), ( int ) in1.height() );
		inp2.copyRect( 0,0, in2, 0, 0, ( int ) in2.width(), ( int ) in2.height() );

		u = flow.updateFlow( inp1 );
		delete u;
		u = flow.updateFlow( inp2 );
		{
			size_t stride;
			uint8_t* base = flowpad.map( &stride );
			u->readData( base, stride );
			flowpad.unmap( base );
			iflow.copyRect( 0,0, flowpad, 0, 0, ( int ) in1.width(), ( int ) in1.height() );
			delete u;
		}

		while( 1 ) {
//			cvShowImage( "Video", iflow.iplimage() );

			key = cvWaitKey( 10 ) & 0xff;
			if( key == 27 )
				break;

		}
		
		FloFile::FloWriteFile( iflow, "out.flo" );
		if( argc == 4 ) {
			Image gt;
			FloFile::FloReadFile( gt, argv[ 3 ] );
			float aee = Flow::AEE( iflow, gt );
			float aae = Flow::AAE( iflow, gt );

			std::cout << "AEE: " << aee << std::endl;
			std::cout << "AAE: " << aae << std::endl;

		}
	} catch( CLException e ) {
		std::cout << e.what() << std::endl;
	} catch( Exception e ) {
		std::cout << e.what() << std::endl;
	}
}
