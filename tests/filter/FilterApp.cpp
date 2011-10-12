#include "FilterApp.h"

#include <cvt/gui/Application.h>
#include <cvt/gui/WidgetLayout.h>
#include <cvt/util/Exception.h>
#include <cvt/gfx/Image.h>
#include <cvt/util/String.h>

#include <iostream>

#include <cvt/gfx/ifilter/BrightnessContrast.h>
#include <cvt/gfx/ifilter/GaussIIR.h>

//#define GAUSSIIR_CL

using namespace cvt;

int FilterApp::run( const String& title )
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

FilterApp::FilterApp( const String & name ) : 
	cvt::TimeoutHandler(),
	_appWindow( name ), 
	_quitButton( "Quit" ),
	_quitDel( &cvt::Application::exit ),
	_mIn( &_inImageView ),
	_mCPU( &_outImageView ),
	_mCL( &_outImageViewCL ),
	_cam( 0 ),
	_filter( 0 ),
	_params( 0 ),
	_framesCPU( 0 ),
	_framesCL( 0 )
{	
	_appWindow.setSize( 800, 600 );
	_appWindow.setVisible( true );
	_appWindow.setMinimumSize( 320, 240 );	
	
	_quitButton.clicked.add( _quitDel );

    cvt::WidgetLayout wl;
    wl.setAnchoredRight( 10, 50 );
    wl.setAnchoredBottom( 10, 20 );
    _appWindow.addWidget( &_quitButton, wl );

	_mIn.setTitle( "Input Image" );
	_mIn.setSize( 320, 240 );
	_appWindow.addWidget( &_mIn );
	_mIn.setPosition( 20, 140 );
	
	_mCPU.setTitle( "Output CPU" );
	_mCPU.setSize( 320, 240 );
	_appWindow.addWidget( &_mCPU );
	_mCPU.setPosition( 360, 140 );
	
	_mCL.setTitle( "Output GPU" );
	_mCL.setSize( 320, 240 );
	_appWindow.addWidget( &_mCL );
	_mCL.setPosition( 360, 140 );

	_timerId = cvt::Application::registerTimer( 100 /* as fast as possible*/, this );
	
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

	_inCL.reallocate( _cam->width(), _cam->height(), IFormat::RGBA_UINT8, IALLOCATOR_CL );
	_outCL.reallocate( _inCL, IALLOCATOR_CL );

	_in.reallocate( _cam->width(), _cam->height(), IFormat::RGBA_UINT8 );
	_out.reallocate( _cam->width(), _cam->height(), IFormat::RGBA_UINT8 );

}

void FilterApp::initFilter()
{
	// select the filter and set parameters
	//_filter = new BrightnessContrast();
	_filter = new GaussIIR();

	_params = _filter->parameterSet();

	/*size_t bloc = _params->paramHandle( "Brightness" );
	_params->setArg<float>( bloc, 0.1f );

	size_t cloc = _params->paramHandle( "Contrast" );
	_params->setArg<float>( cloc, 0.0f );*/

	_outputHandle = _params->paramHandle( "Output" );
	_inputHandle = _params->paramHandle( "Input" );
	size_t sigma = _params->paramHandle( "Sigma" );

	_params->setArg<float>( sigma, 4.0f );

	std::cout << *_params << std::endl;
}

void FilterApp::onTimeout()
{

	try {
		_cam->nextFrame();
		_inImageView.setImage( _cam->frame() );

		_cam->frame().convert( _in );
		_cam->frame().convert( _inCL );

		_params->setArg<Image*>( _inputHandle, &_in );
		_params->setArg<Image*>( _outputHandle, &_out );
		_filter->apply( _params, IFILTER_CPU );
		
		_framesCPU++;
		if( _timerCPU.elapsedSeconds() > 5.0f ) {
			char buf[ 200 ];
			sprintf( buf, "%s CPU: FPS: %.2f", _filter->name().c_str(), _framesCPU / _timerCPU.elapsedSeconds() );
			_mCPU.setTitle( buf );
			_framesCPU = 0;
			_timerCPU.reset();
		}
		
		_outImageView.setImage( _out );
		
		_params->setArg<Image*>( _inputHandle, &_inCL );
		_params->setArg<Image*>( _outputHandle, &_outCL );
		_filter->apply( _params, IFILTER_OPENCL );
		
		_framesCL++;
		if( _timerCL.elapsedSeconds() > 5.0f ) {
			char buf[ 200 ];
			sprintf( buf, "%s CL: FPS: %.2f", _filter->name().c_str(), _framesCL / _timerCL.elapsedSeconds() );
			_mCL.setTitle( buf );
			_framesCL = 0;
			_timerCL.reset();
		}		
		_outImageViewCL.setImage( _outCL );

		
	} catch( CLException e ) {
		std::cout << e.what() << std::endl;
	}
	
}


