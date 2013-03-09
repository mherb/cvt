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
#include <cvt/gfx/ifilter/TVL1Flow.h>
#include <cvt/vision/Flow.h>

#include <cvt/cl/kernel/FlowColorCode.h>

#include <sstream>
#include <iomanip>

using namespace cvt;

class CameraTimeout : public TimeoutHandler
{
    public:
        CameraTimeout( Camera * cam, Moveable* mov, ImageView * imageView ) :
            TimeoutHandler(),
            _cam( cam ),
            _moveable( mov ),
            _view( imageView ),
			_evenodd( 0 ),
            _frames( 0.0f ),
			_flow( 0.9, 10 ),
			_clccode( _FlowColorCode_source, "FlowColorCode" )
        {
            _cam->startCapture();
            _timer.reset();

            _image[ 0 ].reallocate( cam->width(), cam->height(), IFormat::BGRA_UINT8 );
            _image[ 1 ].reallocate( cam->width(), cam->height(), IFormat::BGRA_UINT8 );


        }

        ~CameraTimeout()
        {
            _cam->stopCapture();
        }

        void onTimeout()
        {
            if( !_cam->nextFrame( 20 ) )
                return;

			Image output;
//			Time flowt;

			if( _evenodd ) {
				_cam->frame().convert( _image[ 1 ] );
				_flow.apply( output, _image[ 0 ], _image[ 1 ] );
			} else {
				_cam->frame().convert( _image[ 0 ] );
				_flow.apply( output, _image[ 1 ], _image[ 0 ] );
			}
//			std::cout << flowt.elapsedMilliSeconds() << std::endl;
			_evenodd = !_evenodd;

//			flowt.reset();
//			ccode.reallocate( output.width(), output.height(), IFormat::BGRA_FLOAT );
//			Flow::colorCode( ccode, output, 1.0f );

			Image ccode( output.width(), output.height(), IFormat::BGRA_FLOAT, IALLOCATOR_CL );
			_clccode.setArg( 0, ccode );
			_clccode.setArg( 1, output );
			_clccode.setArg( 2, 30.0f );
			_clccode.run( CLNDRange( Math::pad( output.width(), 16 ), Math::pad( output.height(), 16 ) ), CLNDRange( 16, 16 ) );
            _view->setImage(ccode );

            _frames++;
            if( _timer.elapsedSeconds() > 2.0f ) {
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
        Image		_image[ 2 ];
		int			_evenodd;
        Time		_timer;
        float		_frames;
		TVL1Flow	_flow;
		CLKernel    _clccode;
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
        cam = Camera::get( selection, 320, 240, 60, IFormat::UYVY_UINT8 );
        ImageView camView;
        Moveable m( &camView );
        m.setTitle( "Camera" );
        m.setSize( 320, 240 );
        w.addWidget( &m );

        CameraTimeout camTimeOut( cam, &m, &camView );

        uint32_t timerId = Application::registerTimer( 15, &camTimeOut );
        Application::run();
        Application::unregisterTimer( timerId );

    } catch( cvt::Exception e ) {
        std::cout << e.what() << std::endl;
    }


    if(cam)
        delete cam;

    return 0;
}
