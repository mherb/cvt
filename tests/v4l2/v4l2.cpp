#include <iostream>
#include <opencv/cv.h>
#include <opencv/highgui.h>

#include <cvt/gfx/Image.h>
#include <cvt/gfx/Image.h>
#include <cvt/io/V4L2Camera.h>
#include <cvt/util/Time.h>
#include <cvt/util/Exception.h>
#include <cvt/gfx/ifilter/ROFDenoise.h>
#include <cvt/gfx/IFilterScalar.h>

using namespace cvt;

int main(int argc, char* argv[])
{
	const Image* frame;
	V4L2Camera cam( 0, 640, 480, 30.0, IOrder::BGRA );
	int key;
	size_t frames = 0;
	Time timer;
	bool doprocess = true;
	Image kernel( 3, 1, IOrder::GRAY, IType::FLOAT );
	ROFDenoise rof;
	IFilterParameterSet* rofparam;

	rofparam = rof.getParameterSet();
	IFilterScalar lambda( 0.1f );
	IFilterScalar iter( 50.0f );
	rofparam->setParameter( "Lambda", &lambda );
	rofparam->setParameter( "Iterations", &iter );

	Image x( 640, 480, IOrder::BGRA, IType::FLOAT );
	Image y( 640, 480, IOrder::BGRA, IType::FLOAT );
	Image z( 640, 480, IOrder::BGRA, IType::UBYTE );

	{
		size_t stride;
		uint8_t* ptr;
		float* data;
		ptr = kernel.map( &stride );
		data = ( float* ) ptr;
		*data++ = 1.0f;
		*data++ = 0.0f;
		*data++ = -1.0f;
		kernel.unmap( ptr );
	}

	try {
		cam.open();
		cam.init();
		cam.captureStart();

		timer.reset();
		while( 1 ) {
			cam.captureNext();
			frame = cam.image();

			if( doprocess ) {
				frame->convert( x );
				rofparam->setParameter( "Input", &x );
				rofparam->setParameter( "Output", &y );
				//			rof.apply( y, x, 0.25f, 50 );
				rof.apply( rofparam );
				y.convert( z );
				//			x.scale( y, 1024, 786, IScaleFilterBilinear() );
				/*		x.convolve( y, kernel );
						y = ( y + 1.0f ) * 0.5f;*/
//				cvShowImage( "V4L2", z.iplimage() );
			} else
//				cvShowImage( "V4L2", frame->iplimage() );

			key = cvWaitKey( 10 ) & 0xff;
			if( key == 27 )
				break;
			else if( key == ' ')
				doprocess = !doprocess;

			frames++;
			if( timer.elapsedSeconds() > 5.0f ) {
				std::cout << "FPS: " << ( double ) frames / timer.elapsedSeconds() << std::endl;
				frames = 0;
				timer.reset();
			}
		}
	} catch( Exception e ) {
		std::cout << e.what() << std::endl;
	}

	return 0;
}
