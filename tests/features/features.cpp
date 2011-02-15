#include <cvt/io/Camera.h>
#include <cvt/io/VideoReader.h>
#include <cvt/util/Time.h>
#include <cvt/util/Exception.h>

#include <cvt/gui/Application.h>
#include <cvt/gui/Window.h>
#include <cvt/gui/WidgetLayout.h>
#include <cvt/gui/Moveable.h>
#include <cvt/gui/Button.h>
#include <cvt/gui/BasicTimer.h>
#include <cvt/gui/TimeoutHandler.h>
#include "FeatureView.h"

#include <cvt/io/ImageIO.h>

#include <cvt/vision/FAST.h>
#include <cvt/vision/AGAST.h>

#include "FeatureTracker.h"
#include <algorithm>

using namespace cvt;

class CameraTimeout : public TimeoutHandler
{
	public:
		CameraTimeout( VideoInput * cam, Moveable* mov, FeatureView * view ) :
			TimeoutHandler(),
			_cam( cam ),
			_moveable( mov ),
			_view( view ),
			_image( cam->width(), cam->height(), IFormat::BGRA_UINT8 ),
			_gray( cam->width(), cam->height(), IFormat::GRAY_UINT8 ),
			_frames( 0.0f ),
			_featureTracker()
		{
			_timer.reset();
			_processingTime.reset();
			_iter = 0;
			_timeSum = 0;
		}

		~CameraTimeout()
		{		
		}

		void onTimeout()
		{
			_cam->nextFrame();

			_cam->frame().convert( _gray );

			_view->setImage( _gray );

			std::vector<Feature2D> newFeatures;
			_processingTime.reset();
			_featureTracker.run( _gray, newFeatures );
			_timeSum += _processingTime.elapsedMilliSeconds();

			_iter++;
			if( _iter == 100 ){
				std::cout << "Avg. Feature Detection time: " << _timeSum / _iter << "ms" << std::endl;
				_iter = 0; _timeSum = 0;
			}

			_view->setFeatures( newFeatures, _gray.width(), _gray.height() );

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
		VideoInput *			_cam;
		Moveable*				_moveable;
		FeatureView *			_view;
    
		Image					_image;
		Image					_gray;
    
		Time					_timer;
		Time					_processingTime;
    
		size_t					_iter;
		double					_timeSum;
		float					_frames;
    
		FeatureTracker			_featureTracker;
};

Camera * initCamera()
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
	std::cout << "Select camera: ";
	std::cin >> selection;
	while ( selection >= numCams ){
		std::cout << "Index out of bounds -> select camera in Range:";
		std::cin >> selection;
	}
    
    Camera * cam = 0;
    cam = Camera::get( selection, 640, 480, 60, IFormat::UYVY_UINT8 );
    cam->startCapture();
    
    return cam;
}

int main( int argc, char* argv[] )
{
    VideoInput * input = 0;
	if( argc == 1 ){
        input = initCamera();
    } else {
        input = new VideoReader( argv[ 1 ], true );
    }

	Window w( "Camera Test" );
	w.setSize( 800, 600 );
	w.setVisible( true );
	w.setMinimumSize( 320, 240 );

	Button button( "Quit" );
	Delegate<void ()> dquit( &Application::exit );
	button.clicked.add( &dquit );
    WidgetLayout wl;
    wl.setAnchoredRight( 10, 50 );
    wl.setAnchoredBottom( 10, 20 );
    w.addWidget( &button, wl );    

	try {		
		FeatureView camView;
		Moveable m( &camView );
		m.setTitle( "Camera" );
		m.setSize( 320, 240 );
		w.addWidget( &m );

		CameraTimeout camTimeOut( input, &m, &camView );
		uint32_t timerId = Application::registerTimer( 10, &camTimeOut );
		Application::run();
		Application::unregisterTimer( timerId );

	} catch( cvt::Exception e ) {
		std::cout << e.what() << std::endl;
	}
    
    if( input )
		delete input;

	return 0;
}
