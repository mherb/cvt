#include <iostream>
#include <cv.h>
#include <highgui.h>
#include <list>

#include <cvt/gfx/Image.h>
#include <cvt/gfx/Image.h>
#include <cvt/io/DC1394Camera.h>
#include <cvt/io/ImageIO.h>
#include <cvt/util/Time.h>
#include <cvt/util/Exception.h>

using namespace cvt;

#define FPS ( 1.0f / 25.0f )

int main(int argc, char* argv[])
{
	CameraInfo info;
	DC1394Camera::cameraInfo( 0, info );
	const CameraMode & mode = info.bestMatchingMode( IFormat::RGBA_UINT8, 640, 480, 30 );
	
	DC1394Camera cam( 0, mode );

	int key;
	size_t frames = 0;
	Time timer;
	size_t seq = 0;
	char buffer[ 512 ];
	Image* out;
	std::list<Image*> list;
	
	try {
		cam.startCapture();

		timer.reset();
		while( 1 ) {
			cam.nextFrame();
			const Image & frame = cam.frame();

			out = new Image( 640, 480, IFormat::GRAY_UINT8 );
			frame.convert( *out );

			// cvShowImage( "DC1394", out->iplimage() );
			list.push_back( out );

			key = cvWaitKey( 10 ) & 0xff;
			if( key == 27 )
				break;

			frames++;
			if( timer.elapsedSeconds() > 5.0f ) {
				std::cout << "FPS: " << ( double ) frames / timer.elapsedSeconds() << std::endl;
				frames = 0;
				timer.reset();
			}
		}
		
		cam.stopCapture();
	} catch( cvt::Exception e ) {
		std::cout << e.what() << std::endl;
	}

	while( !list.empty() ) {
		out = list.front();
		list.pop_front();
		sprintf( buffer, "frame_%03lu.png", seq++ );
		ImageIO::savePNG( *out, buffer );
		delete out;
	}


	return 0;
}
