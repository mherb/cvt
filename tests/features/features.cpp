#include <cvt/io/Camera.h>
#include <cvt/io/VideoReader.h>
#include <cvt/io/ImageSequence.h>
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

class CameraApp : public Window
{
	public:
		CameraApp( VideoInput * cam ) : 
            Window( "Camera Application" ),
			_cam( cam ),		
            _paused( false ),
            _captureNext( true ),
            _quitButton( 0 ),
			_image( cam->width(), cam->height(), IFormat::BGRA_UINT8 ),
			_gray( cam->width(), cam->height(), IFormat::GRAY_UINT8 ),
			_frames( 0.0f ),
			_featureTracker(),
            _loopTimer( 5 )
		{
            initGuiElements();
            
			_timer.reset();
			_processingTime.reset();
			_iter = 0;
			_timeSum = 0;
            
            _loopDelegate = new Delegate<void ( BasicTimer* )>( this, &CameraApp::runloop );
            _loopTimer.timeout.add( _loopDelegate );
            _loopTimer.start();
		}
    
        void initGuiElements()
        {
            setSize( 800, 600 );
            
            _featureView = new FeatureView();
            
            _moveable = new Moveable( _featureView );
            _moveable->setSize( 320, 240 );
            addWidget( _moveable );
            
            _quitButton = new Button( "Quit" );
            _quit = new Delegate<void ()>( &Application::exit );           
            _quitButton->clicked.add( _quit );
            
            _pauseButton = new Button( "Pause" );
            _pauseDelegate = new Delegate<void ()>( this, &CameraApp::pauseClicked );
            _pauseButton->clicked.add( _pauseDelegate );

            _nextButton = new Button( "Next Frame" );
            _nextDelegate = new Delegate<void ()>( this, &CameraApp::nextClicked );
            _nextButton->clicked.add( _nextDelegate );            
            
            WidgetLayout wl;
            wl.setAnchoredRight( 10, 70 );
            wl.setAnchoredBottom( 10, 20 );
            addWidget( _quitButton, wl );
            wl.setAnchoredBottom( 40, 20 );
            addWidget( _nextButton, wl );
            wl.setAnchoredBottom( 70, 20 );
            addWidget( _pauseButton, wl );
            
            setVisible( true );
        }

		~CameraApp()
		{		
            _loopTimer.timeout.remove( _loopDelegate );
            delete _loopDelegate;
            
            _quitButton->clicked.remove( _quit );
            delete _quit;
            delete _quitButton;            
		}
    
        void pauseClicked()
        {
            _paused ^= 1;            
            _captureNext = false;
        }
    
        void nextClicked()
        {
            _captureNext = true;
        }

		void runloop( BasicTimer* )
		{
            if( !_paused || _captureNext ){
                _cam->nextFrame();
                _captureNext ^= 1;
            }
			_cam->frame().convert( _gray );
			_featureView->setImage( _gray );

			std::vector<Feature2D> newFeatures;
			_processingTime.reset();
			_featureTracker.run( _gray, newFeatures );
			_timeSum += _processingTime.elapsedMilliSeconds();

			_iter++;
			if( _iter == 100 ){
				std::cout << "Avg. Feature Detection time: " << _timeSum / _iter << "ms" << std::endl;
				_iter = 0; _timeSum = 0;
			}

			_featureView->setFeatures( newFeatures, _gray.width(), _gray.height() );

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
        bool                    _paused;
        bool                    _captureNext;
    
		FeatureView *			_featureView;
        Moveable*				_moveable;
        Button*                 _quitButton;
        Button*                 _pauseButton;
        Button*                 _nextButton;
    
		Image					_image;
		Image					_gray;
    
		Time					_timer;
		Time					_processingTime;
    
		size_t					_iter;
		double					_timeSum;
		float					_frames;    
		FeatureTracker          _featureTracker;
    
        BasicTimer                         _loopTimer;
    
        // delegates
        Delegate<void ( BasicTimer* ) >*   _loopDelegate;
        Delegate<void () >*                _quit;
        Delegate<void () >*                _pauseDelegate;
        Delegate<void () >*                _nextDelegate;
    
        
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
        //input = new VideoReader( argv[ 1 ], true );        
        std::string ext( "cvtraw" );
        input = new ImageSequence( argv[ 1 ], ext, 1, 2680, 5 );
        
    }

	try {
		CameraApp camTimeOut( input );
		Application::run();
	} catch( cvt::Exception e ) {
		std::cout << e.what() << std::endl;
	}
    
    if( input )
		delete input;

	return 0;
}
