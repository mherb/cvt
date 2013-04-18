#include <FeaturesHomographyApp.h>
#include <cvt/gui/Application.h>

namespace cvt {

    FeaturesHomographyApp::FeaturesHomographyApp( VideoInput* cam,
                                                  const cvt::Image& refImage ) :
        _cam( cam ),
        _timerId( 0 ),
        _iterations( 0 ),
        _mainWindow( "Features Homography" )
    {
        _timerId = Application::registerTimer( 10, this );
    }

    FeaturesHomographyApp::~FeaturesHomographyApp()
    {
    }

    void FeaturesHomographyApp::onTimeout()
    {
        _cam->nextFrame();

		Image gray;
		_cam->frame().convert( gray, IFormat::GRAY_UINT8 );

		// compute the features

		// update fps

	}

    void FeaturesHomographyApp::updateFPS()
    {
        _iterations++;
        if( _iterations == 30 ){
            float fps = (float)_iterations / _processingTime.elapsedSeconds();
            _iterations = 0;
            _processingTime.reset();
            String title;
            title.sprintf( "FPS: %0.1f", fps );
            //_moveable.setTitle( title );
        }
    }

	void FeaturesHomographyApp::initGuiElements()
	{
		_mainWindow.setSize( 800, 600 );

		/*
		_moveable.setSize( 320, 240 );
		addWidget( &_moveable );

		_templateMov.setSize( 320, 240 );
		addWidget( &_templateMov );

		_quitButton.clicked.add( Delegate<void ()>( &Application::exit ) );

		WidgetLayout wl;
		wl.setAnchoredRight( 10, 100 );
		wl.setAnchoredBottom( 10, 20 );
		addWidget( &_quitButton, wl );
		wl.setAnchoredBottom( 40, 20 );

		_moveable.setTitle( "ORB Estimate" );
		_templateMov.setTitle( "Template Image" );
		*/

		_mainWindow.setVisible( true );
	}

}
