#include <cvt/io/Camera.h>
#include <cvt/util/Time.h>
#include <cvt/util/Exception.h>

#include <cvt/gui/Application.h>
#include <cvt/gui/Window.h>
#include <cvt/gui/WidgetLayout.h>
#include <cvt/gui/Moveable.h>
#include <cvt/gui/Button.h>
#include <cvt/gui/ImageView.h>
#include <cvt/gui/BasicTimer.h>
#include <cvt/gui/TimeoutHandler.h>

#include <cvt/util/Thread.h>
#include <cvt/util/TQueue.h>

#include <sstream>
#include <iomanip>

using namespace cvt;

//#define WRITE_PNGS


class ImageWriter : public Thread<void>
{
	public:
		ImageWriter( const std::string & baseName, TQueue<Image*> * inqueue ) : _baseName( baseName), _queue( inqueue )
		{
		}

		~ImageWriter()
		{
		}

		void execute(void*)
		{
			std::stringstream ss;
			size_t i = 0;
			while( true ){
				Image * im = _queue->waitNext();

				if( im == 0 )
					break;

				i++;
				ss.clear();
				ss.str( "" );
				ss << _baseName << "_" << std::setfill( '0' ) << std::setw( 5 ) << i << ".cvtraw";

				im->save( ss.str().c_str() );

				delete im;
			}
		}

	private:
		std::string		_baseName;
		TQueue<Image*>*	_queue;

};

class CameraTimeout : public TimeoutHandler
{
	public:
		CameraTimeout( Camera * cam, Moveable* mov, ImageView * imageView ) :
			TimeoutHandler(),
			_cam( cam ),
			_moveable( mov ),
			_view( imageView ),
			_image( cam->width(), cam->height(), IFormat::BGRA_UINT8 ),
			_frames( 0.0f )
		{
			_cam->startCapture();
			_timer.reset();

#ifdef WRITE_PNGS
			_writer = new ImageWriter( "image", &_queue );
			_writer->run( NULL );
#endif
		}

		~CameraTimeout()
		{
			_cam->stopCapture();
#ifdef WRITE_PNGS
			_queue.enqueue( 0 );
			_writer->join();
			delete _writer;
#endif
		}

		void onTimeout()
		{
			Time camt;
			camt.reset();
			_cam->nextFrame();

#ifdef WRITE_PNGS
			Image * toSave = new Image( _cam->frame() );
			_queue.enqueue( toSave );
#endif
			_view->setImage( _cam->frame() );
			if( camt.elapsedMicroSeconds() > 3000.0 )
				std::cout << camt.elapsedMicroSeconds() << std::endl;
			_frames++;
			if( _timer.elapsedSeconds() > 5.0f ) {
				char buf[ 200 ];
				sprintf( buf,"Camera FPS: %.2f", _frames / _timer.elapsedSeconds() );
				_moveable->setTitle( buf );
				_frames = 0;
				_timer.reset();
			}
		}

	private:
		Camera *	_cam;
		Moveable*   _moveable;
		ImageView *	_view;
		Image		_image;
		Time		_timer;
		float		_frames;

#ifdef WRITE_PNGS
		ImageWriter*	_writer;
		TQueue<Image*>	_queue;
#endif
};

int main( )
{
	Camera::updateInfo();
	size_t numCams = Camera::count();

	std::cout << "Overall number of Cameras: " << numCams << std::endl;
	if( numCams == 0 ){
		std::cout << "Please connect a camera!" << std::endl;
		return 0;
	}

	for( size_t i = 0; i < numCams; i++ ){
		const CameraInfo & info = Camera::info( i );
		std::cout << "Camera " << i << ": " << info << std::endl;
	}

	size_t selection = numCams;
	if( numCams == 1 )
		selection = 0;
	else {
		std::cout << "Select camera: ";
		std::cin >> selection;
		while ( selection >= numCams ){
			std::cout << "Index out of bounds -> select camera in Range:";
			std::cin >> selection;
		}
	}

	Window w( "Camera Test" );
	w.setSize( 800, 600 );
	w.setVisible( true );
	w.setMinimumSize( 320, 240 );

	Button button( "Quit" );
	Delegate<void ()> dquit( &Application::exit );
	button.clicked.add( dquit );
    WidgetLayout wl;
    wl.setAnchoredRight( 10, 50 );
    wl.setAnchoredBottom( 10, 20 );
    w.addWidget( &button, wl );

	Camera * cam = 0;

	try {
		cam = Camera::get( selection, 640, 480, 60, IFormat::UYVY_UINT8 );
		ImageView camView;
		Moveable m( &camView );
		m.setTitle( "Camera" );
		m.setSize( 320, 240 );
		w.addWidget( &m );

		CameraTimeout camTimeOut( cam, &m, &camView );
		uint32_t timerId = Application::registerTimer( 10, &camTimeOut );
		Application::run();
		Application::unregisterTimer( timerId );

	} catch( cvt::Exception e ) {
		std::cout << e.what() << std::endl;
	}


	if(cam)
		delete cam;

	return 0;
}
