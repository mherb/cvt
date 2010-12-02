#include <cvt/io/VideoReader.h>
#include <cvt/io/Resources.h>
#include <cvt/util/Time.h>
#include <cvt/util/Exception.h>

#include <cv.h>
#include <highgui.h>

using namespace cvt;

int main( int argc, char** argv )
{
	int key;
	size_t frames = 0;
	Time timer;
	std::string xxx( argv[ 1 ] );

	try {
		VideoReader video( xxx );

		Image frame( video.width(), video.height(), IFormat::BGRA_UINT8 );

		timer.reset();
		while( 1 ) {
			video.nextFrame();
			video.frame().convert( frame );

			size_t stride;
			const uint8_t * data = frame.map( &stride );
			cv::Mat ocvImage( (int)frame.height(), (int)frame.width(), CV_8UC4, (void *)data, stride );
			cv::imshow( "frame", ocvImage );
			frame.unmap( data );

			key = cvWaitKey( 3 ) & 0xff;
			if( key == 27 )
				break;

			frames++;
			if( timer.elapsedSeconds() > 5.0f ) {
				std::cout << "FPS: " << ( double ) frames / timer.elapsedSeconds() << std::endl;
				frames = 0;
				timer.reset();
			}
		}
	} catch( cvt::Exception e ) {
		std::cout << e.what() << std::endl;
	}

	return 0;
}
