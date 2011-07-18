/*
 * File:   VideoInputGui.h
 * Author: sebi
 *
 * Created on July 15, 2011, 9:22 AM
 */

#ifndef VIDEOINPUTGUI_H
#define	VIDEOINPUTGUI_H

namespace cvt
{

class CameraApp : public Window
{
	public:
		CameraApp( VideoInput * cam ) :
            Window( "Camera Application" ),
            _moveable( _imView ),
            _quitButton( "Quit" ),
			_cam( cam ),
            _quitButton( 0 ),
            _loopTimer( 5 )
		{
            initGuiElements();

			_timer.reset();
			_processingTime.reset();

            Delegate<void ( BasicTimer* )> loopDelegate( this, &CameraApp::runloop );
            _loopTimer.timeout.add( loopDelegate );
            _loopTimer.start();
		}

        void initGuiElements()
        {
            setSize( 800, 600 );

            _moveable.setSize( 320, 240 );
            addWidget( &_moveable );

            _quitButton.clicked.add( Delegate<void ()>( &Application::exit ) );

            WidgetLayout wl;
            wl.setAnchoredRight( 10, 100 );
            wl.setAnchoredBottom( 10, 20 );
            addWidget( _quitButton, wl );
            wl.setAnchoredBottom( 40, 20 );

            setVisible( true );
        }

		~CameraApp()
		{
            _loopTimer.timeout.remove( Delegate<void ( BasicTimer* )>( this, &CameraApp::runloop ) );
            _quitButton->clicked.remove( Delegate<void ()>( &Application::exit ) );
		}

		void runloop( BasicTimer* )
		{
            _cam->nextFrame();

            Image gray;
			_cam->frame().convert( gray, IFormat::GRAY_UINT8 );
			_imView.setImage( gray );

			_processingTime.reset();
        }

	private:
		VideoInput*  _cam;

        Moveable	 _moveable;
        ImageView    _imView;
        Button       _quitButton;

		Time		_timer;
		Time		_processingTime;

        BasicTimer  _loopTimer;

        ORB         _orbModel;

};

}



#endif	/* VIDEOINPUTGUI_H */

