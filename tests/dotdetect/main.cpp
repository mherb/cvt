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

#include <cvt/gfx/GFXEngineImage.h>

#include <sstream>
#include <iomanip>

#include <cvt/vision/BoardDetector.h>

using namespace cvt;

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
        }

        ~CameraTimeout()
        {
            _cam->stopCapture();
        }

        void onTimeout()
        {
            if( !_cam->nextFrame( 20 ) )
                return;

			Image in1, in2, out, out2;
			_cam->frame().convert( in1, IFormat::RGBA_UINT8 );
			in1.convert( in2, IFormat::GRAY_FLOAT );


			BoardDetector detector;
			PointSet2f pts;

			if( detector.detectACirclePattern( pts, in2, 4, 11 ) )
			{
				GFXEngineImage ge( in1 );
				GFX g( &ge );
				g.color().set( 1.0f, 0.0f, 0.0f, 1.0f );

				for( size_t i = 0; i < pts.size(); i++ ) {
						g.drawLine( pts[ i ] + Vector2f(-2,0), pts[ i ] + Vector2f(2,0) );
						g.drawLine( pts[ i ] + Vector2f(0,-2), pts[ i ] + Vector2f(0,2) );
				}
			}

            _view->setImage( in1 );

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
		Canny		_canny;
        Image		_image;
        Time		_timer;
        float		_frames;
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
