#include <cvt/io/Camera.h>
#include <cvt/io/UEyeUsbCamera.h>
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

class CameraTimeout : public TimeoutHandler
{
	public:
		CameraTimeout( UEyeUsbCamera * cam, Moveable* mov, ImageView * imageView ) :
			TimeoutHandler(),
			_cam( cam ),
			_moveable( mov ),
			_view( imageView ),
			_image( cam->width(), cam->height(), IFormat::BGRA_UINT8 ),
			_frames( 0.0f ),
			_dump( false ),
			_dumpIter( 0 )
		{
            _cam->setFramerate( 30 );
			_cam->startCapture();

            //_cam->setRunMode( UEyeUsbCamera::UEYE_MODE_HW_TRIGGER );
            _cam->setRunMode( UEyeUsbCamera::UEYE_MODE_TRIGGERED );

            _cam->setFlashMode( UEyeUsbCamera::FLASH_LOW_ON_EXPOSURE );
            _cam->setFlashDelayAndDuration( 0, 40 );

            //_cam->setTriggerDelay( 0 );

			_cam->setHardwareGains( 0, 8, 0, 14 );
			_timer.reset();

            //_cam->testIOSupport();
		}

		~CameraTimeout()
		{
			_cam->stopCapture();
		}

		void onTimeout()
		{
            _cam->nextFrame();
			_view->setImage( _cam->frame() );

			_frames++;
			if( _timer.elapsedSeconds() > 0.3f ) {
				char buf[ 200 ];
				sprintf( buf,"Camera FPS: %.2f", _frames / _timer.elapsedSeconds() );
				_moveable->setTitle( buf );
				_frames = 0;
				_timer.reset();
                //showGains();
			}

			if( _dump ){
				_dumpIter++;

				String name;
				name.sprintf( "camera_image_%03d.png", _dumpIter );
			
				Image img;
				_cam->frame().convert( img, IFormat::RGBA_UINT8 );

				img.save( name );

				_dump = false;
			}
		}

		void setDump(){ _dump = true; }

		void showGains()
		{
			int m, r, g, b;
			_cam->getHardwareGains( m, r, g, b );
			std::cout << "Master: " << m << ", Red: " << r << ", Green: " << g << ", Blue: " << b << std::endl;
		}

		void updateGains()
		{
			int m, r, g, b;
			_cam->getHardwareGains( m, r, g, b );
			std::cout << "Before -> Master: " << m << ", Red: " << r << ", Green: " << g << ", Blue: " << b << std::endl;
			if( r == 100 )
				r = 0;
			else 
				r++;
			_cam->setHardwareGains( m, r, g, b );
			_cam->getHardwareGains( m, r, g, b );
			std::cout << "After -> Master: " << m << ", Red: " << r << ", Green: " << g << ", Blue: " << b << std::endl;
		}

	private:
		UEyeUsbCamera*	_cam;
		Moveable*   _moveable;
		ImageView *	_view;
		Image		_image;
		Time		_timer;
		float		_frames;

		bool		_dump;
		size_t		_dumpIter;
};

int main( )
{
	size_t numCams = UEyeUsbCamera::count();
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

	UEyeUsbCamera * cam = 0;

	try {
		CameraInfo camInfo;
		UEyeUsbCamera::cameraInfo( selection, camInfo ); 
		CameraMode mode = camInfo.bestMatchingMode( IFormat::BAYER_RGGB_UINT8, 640, 480, 30 );
		cam = new UEyeUsbCamera( camInfo.index(), mode );
		ImageView camView;
		Moveable m( &camView );
		m.setTitle( "Camera" );
		m.setSize( 320, 240 );
		w.addWidget( &m );

		CameraTimeout camTimeOut( cam, &m, &camView );

		Button dumpButton( "Save" );
		Delegate<void ()> dump( &camTimeOut, &CameraTimeout::setDump );
		dumpButton.clicked.add( dump );
		wl.setAnchoredBottom( 40, 20 );
		w.addWidget( &dumpButton, wl );

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
