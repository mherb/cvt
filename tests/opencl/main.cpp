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

#include "CLOptflow.h"

using namespace cvt;

class FlowCameraTimeout : public TimeoutHandler
{
	public:
		FlowCameraTimeout( Camera * cam, Moveable* mov, ImageView * imageView, ImageView* flowView ) :
			TimeoutHandler(),
			_cam( cam ),
			_moveable( mov ),
			_view( imageView ),
			_view2( flowView ),
			_image( cam->width(), cam->height(), IFormat::BGRA_UINT8 ),
			_frames( 0.0f )
		{
			_cam->startCapture();
			_timer.reset();
			_cl.makeCurrent();
			_optflow = new CLOptflow();
		}

		~FlowCameraTimeout()
		{
			_cam->stopCapture();
			delete _optflow;
		}

		void onTimeout()
		{
			_cam->nextFrame();

			Image* flow;
			try {
				Image input( 640, 480, IFormat::GRAY_UINT8 );
				_cam->frame().convert( input );
				flow = _optflow->updateFlow( input );
			} catch( CLException& e ) {
				std::cout << e.what() << std::endl;
			}

			_view->setImage( _cam->frame() );
			_view2->setImage( *flow );
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
		ImageView *	_view2;
		Image		_image;
		Time		_timer;
		float		_frames;
		CLContext	_cl;
		CLOptflow*  _optflow;
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
		m.setTitle( "Input" );
		m.setSize( 320, 240 );
		w.addWidget( &m );

		ImageView flowView;
		Moveable m2( &flowView );
		m2.setTitle( "Optflow" );
		m2.setSize( 320, 240 );
		w.addWidget( &m2 );

		FlowCameraTimeout camTimeOut( cam, &m, &camView, &flowView );

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
