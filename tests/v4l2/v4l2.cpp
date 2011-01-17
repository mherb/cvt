#include <iostream>

#include <cvt/gfx/Image.h>
#include <cvt/gfx/Image.h>
#include <cvt/io/V4L2Camera.h>
#include <cvt/util/Time.h>
#include <cvt/util/Exception.h>
#include <cvt/util/ParamSet.h>
#include <cvt/gfx/ifilter/ROFDenoise.h>

using namespace cvt;

int main(int argc, char* argv[])
{
	CameraInfo camInfo;
	std::cout << "V4L2 cameras: " << std::endl;
	for( size_t i = 0; i < V4L2Camera::count(); i++ ){
		V4L2Camera::cameraInfo( i, camInfo );
		CameraModeSet modeSet = camInfo.modeSet().filter( IFormat::YUYV_UINT8, 640, 480, 25 );
		std::cout << "Camera: " << camInfo.name() << "\n" << modeSet << std::endl;
	}

	V4L2Camera::cameraInfo( 0, camInfo );
	CameraMode mode = camInfo.bestMatchingMode( IFormat::BGRA_UINT8, 640, 480, 30 );
	std::cout << "Selected mode: " << mode << std::endl;

	V4L2Camera cam( 0, mode );

	int key = 0;
	size_t frames = 0;
	Time timer;
	bool doprocess = true;
	Image kernel( 3, 1, IFormat::GRAY_FLOAT );
	ROFDenoise rof;
	ParamSet* rofparam;

	rofparam = rof.parameterSet();
	size_t handle = rofparam->paramHandle("Lambda");
	rofparam->setArg( handle, 0.1f );
	handle = rofparam->paramHandle("Iterations");
	rofparam->setArg( handle, 50 );

	Image x( 640, 480, IFormat::BGRA_FLOAT );
	Image y( 640, 480, IFormat::BGRA_FLOAT );
	Image z( 640, 480, IFormat::BGRA_UINT8 );

	Image frame( mode.width, mode.height, IFormat::BGRA_UINT8 );

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
		cam.startCapture();

		timer.reset();
		while( 1 ) {
			cam.nextFrame();
			cam.frame().convert( frame );

			if( doprocess ) {
				frame.convert( x );
				rofparam->setArg( rofparam->paramHandle("Input"), &x );
				rofparam->setArg( rofparam->paramHandle("Output"), &y );
				//			rof.apply( y, x, 0.25f, 50 );
				rof.apply( rofparam );
				y.convert( z );
				//			x.scale( y, 1024, 786, IScaleFilterBilinear() );
				/*		x.convolve( y, kernel );
						y = ( y + 1.0f ) * 0.5f;*/
//				cvShowImage( "V4L2", z.iplimage() );
			} else
//				cvShowImage( "V4L2", frame->iplimage() );

/*			key = cvWaitKey( 10 ) & 0xff;
			if( key == 27 )
				break;
			else if( key == ' ')
				doprocess = !doprocess;*/

			frames++;
			if( timer.elapsedSeconds() > 5.0f ) {
				std::cout << "FPS: " << ( double ) frames / timer.elapsedSeconds() << std::endl;
				frames = 0;
				timer.reset();
			}
		}

		cam.stopCapture();

	} catch( Exception e ) {
		std::cout << e.what() << std::endl;
	}

	return 0;
}
