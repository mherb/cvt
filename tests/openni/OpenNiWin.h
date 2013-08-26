#ifndef CVT_OPENNI_WIN_H
#define CVT_OPENNI_WIN_H

#include <cvt/gui/TimeoutHandler.h>
#include <cvt/gui/Window.h>
#include <cvt/gui/ImageView.h>
#include <cvt/gui/Moveable.h>
#include <cvt/io/OpenNI2Camera.h>
#include <cvt/util/Time.h>

namespace cvt
{
    class OpenNiWin : public TimeoutHandler
    {
        public:
            OpenNiWin();
            ~OpenNiWin();

            void onTimeout();

        private:
            uint32_t		_timerId;
            Window			_window;
            ImageView		_rgbView;
            ImageView		_depthView;
            Moveable		_rgbMov;
            Moveable		_depthMov;
            OpenNI2Camera	_cam;

            Time			_time;
            size_t			_iters;
    };

    inline OpenNiWin::OpenNiWin() :
        _window( "OpenNI" ),
        _rgbMov( &_rgbView ),
        _depthMov( &_depthView ),
        _cam( 0, CameraMode( 640, 480, 30, IFormat::UYVY_UINT8 ) )
    {
        _timerId = Application::registerTimer( 10, this );

        _cam.setAutoExposure( true );
        _cam.setAutoWhiteBalance( true );

        _cam.setRegisterDepthToRGB( true );
        _cam.setSyncRGBDepth( false );
        _cam.startCapture();

        _window.setSize( 800, 600 );
        _depthMov.setSize( 320, 240 );
        _rgbMov.setSize( 320, 240 );


        _depthMov.setTitle( "Depth" );

        _rgbMov.setTitle( "Image" );

        _window.addWidget( &_depthMov );
        _window.addWidget( &_rgbMov );

        _window.setVisible( true );
        _window.update();

    }

    inline OpenNiWin::~OpenNiWin()
    {
        Application::unregisterTimer( _timerId );
        _cam.stopCapture();
    }

    inline void OpenNiWin::onTimeout()
    {
        _cam.nextFrame();

        _iters++;
        double t;
        if( ( t = _time.elapsedSeconds() ) > 3.0 ){
            String title;
            title.sprintf( "OpenNI - FPS: %0.1f", _iters / t );
            _window.setTitle( title );
            _iters = 0;
            _time.reset();

            _cam.setAutoExposure( !_cam.autoExposure() );
        }

        Image img;
        _cam.frame().convert( img, IFormat::RGBA_UINT8 );

        Image dFloat;
        _cam.depth().convert( dFloat, IFormat::GRAY_FLOAT );
        dFloat.mul( 6.0f );
        _rgbView.setImage( img );
        _depthView.setImage( dFloat );
    }
}

#endif
