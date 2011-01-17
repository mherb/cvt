#include "FilterApp.h"

#include <cvt/gui/Application.h>
#include <cvt/gui/WidgetLayout.h>
#include <cvt/util/Exception.h>
#include <cvt/gfx/Image.h>

#include <iostream>

#include <cvt/gfx/ifilter/BrightnessContrast.h>
#include <cvt/io/ImageIO.h>

using namespace cvt;

int FilterApp::run( const std::string & title )
{
	try {
		FilterApp app( title );
		cvt::Application::run();
	} catch( const cvt::Exception & e ) {
		std::cerr << e.what() << std::endl;
		return 1;
	}
	
	return 0;
}

FilterApp::FilterApp( const std::string & name ) : 
	cvt::TimeoutHandler(),
	_appWindow( name ), 
	_quitButton( "Quit" ),
	_quitDel( &cvt::Application::exit ),
	_mIn( &_inImageView ),
	_mOut( &_outImageView ),
	_cam( 0 ),
	_filter( 0 ),
	_params( 0 ),
	_frames( 0 )
{	
	_appWindow.setSize( 800, 600 );
	_appWindow.setVisible( true );
	_appWindow.setMinimumSize( 320, 240 );	
	
	_quitButton.clicked.add( &_quitDel );

    cvt::WidgetLayout wl;
    wl.setAnchoredRight( 10, 50 );
    wl.setAnchoredBottom( 10, 20 );
    _appWindow.addWidget( &_quitButton, wl );

	_mIn.setTitle( "Input Image" );
	_mIn.setSize( 320, 240 );
	_appWindow.addWidget( &_mIn );
	_mIn.setPosition( 20, 140 );
	
	_mOut.setTitle( "Output Image" );
	_mOut.setSize( 320, 240 );
	_appWindow.addWidget( &_mOut );
	_mOut.setPosition( 360, 140 );

	_timerId = cvt::Application::registerTimer( 30 /* as fast as possible*/, this );
	
	_cl.makeCurrent();
	initCamera();
	initFilter();
}

FilterApp::~FilterApp()
{
	if( _cam ){
		_cam->stopCapture();
		delete _cam;
	}

	if( _params )
		delete _params;
		
	cvt::Application::unregisterTimer( _timerId );
	_timer.reset();
}

void FilterApp::initCamera()
{
	cvt::Camera::updateInfo();
	size_t numCams = cvt::Camera::count();

	std::cout << "Overall number of Cameras: " << numCams << std::endl;
	if( numCams == 0 ){
		throw CVTException( "Please Connect a supported camera first!" );
	}

	for( size_t i = 0; i < numCams; i++ ){
		const cvt::CameraInfo & info = cvt::Camera::info( i );
		std::cout << "Camera " << i << ": " << info << std::endl;
	}

	size_t selection = numCams;
	
	if( numCams == 1 ){
		selection = 0;
	} else {
		std::cout << "Select camera: ";
		std::cin >> selection;
		while ( selection >= numCams ){
			std::cout << "Index out of bounds -> select camera in Range:";
			std::cin >> selection;
		}
	}
	
	_cam = cvt::Camera::get( selection, 640, 480, 60, cvt::IFormat::UYVY_UINT8 );
	_cam->startCapture();
	_cam->nextFrame();
	
	std::cout << _cam->frame() << std::endl;
	
	_inCL.reallocate( _cam->frame(), IALLOCATOR_CL );
	_outCL.reallocate( _inCL, IALLOCATOR_CL );
}

void FilterApp::initFilter()
{
	// select the filter and set parameters	
	_filter = new BrightnessContrast();
	_filterType = IFILTER_OPENCL;
	_params = _filter->parameterSet();
	
	size_t bloc = _params->paramHandle( "Brightness" );
	_params->setArg<float>( bloc, 0.1f );
	
	size_t cloc = _params->paramHandle( "Contrast" );
	_params->setArg<float>( cloc, 0.0f );
	
	_outputHandle = _params->paramHandle( "Output" );
	_params->setArg<Image*>( _outputHandle, &_outCL );
	
	_inputHandle = _params->paramHandle( "Input" );
	_params->setArg<Image*>( _inputHandle, &_inCL );

	std::cout << *_params << std::endl;
}

void FilterApp::onTimeout()
{

	try {
		_cam->nextFrame();
		_inImageView.setImage( _cam->frame() );

		_inCL.copy( _cam->frame() );

		_params->setArg<Image*>( _inputHandle, &_inCL );
		_params->setArg<Image*>( _outputHandle, &_outCL );

		_filter->apply( _params, _filterType );

		_outImageView.setImage( _outCL );

		_frames++;
	} catch( CLException e ) {
		std::cout << e.what() << std::endl;
	}
	
	if( _timer.elapsedSeconds() > 5.0f ) {
		char buf[ 200 ];
		sprintf( buf, "%s: FPS: %.2f", _filter->name().c_str(), _frames / _timer.elapsedSeconds() );
		_mOut.setTitle( buf );
		_frames = 0;
		_timer.reset();
	}
}


