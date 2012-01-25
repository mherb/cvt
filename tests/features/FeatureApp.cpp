#include "FeatureApp.h"

#include <cvt/gui/Application.h>

#include <cvt/vision/ORB.h>
#include <cvt/gfx/GFX.h>
#include <cvt/gfx/GFXEngineImage.h>

namespace cvt
{

	FeatureApp::FeatureApp( VideoInput & video ) :
		_window( "Features" ),	
		_video( video ),
		_fps( 0.0 ),
		_iter( 0 ),
		_avgDetectorTime( 0.0 ),
		_allIter( 0 )
	{
		_timerId = Application::registerTimer( 33, this );
		_window.setSize( 640, 480 );
		WidgetLayout wl;
		wl.setAnchoredTopBottom( 0, 0 );
		wl.setAnchoredLeftRight( 0, 0 );
		_window.addWidget( &_imView, wl );
		_window.setVisible( true );
		_window.update();				
	}

	FeatureApp::~FeatureApp()
	{
		Application::unregisterTimer( _timerId );
	}

	static void drawFeatures( Image& img, const ORB& orb )
	{
		GFXEngineImage ge( img );
		GFX g( &ge );
		g.color() = Color::GREEN;

		Recti r( 0, 0, 3, 3 );
		for( size_t i = 0; i < orb.size(); i++ ){
			r.x = orb[ i ].pt.x - 1;
			r.y = orb[ i ].pt.y - 1;
			g.drawRect( r );
		}
	}

	void FeatureApp::onTimeout()
	{
		_video.nextFrame();
		_video.frame().convert( _gray, IFormat::GRAY_UINT8 );

		Time detectTime;
		ORB orb( _gray  );
		_avgDetectorTime += detectTime.elapsedMilliSeconds();

		Image debug;
		_video.frame().convert( debug, IFormat::RGBA_UINT8 );
		drawFeatures( debug, orb );
		_imView.setImage( debug );

		_iter++;
		_allIter++;

		double t = _time.elapsedSeconds();
		if( t > 3.0 ){
			String title;
			title.sprintf( "Features - FPS %0.1f - Avg. Detection time: %0.1fms" , _iter / t, _avgDetectorTime / _allIter );
			_window.setTitle( title );
			_iter = 0;
			_time.reset();
		}
	}
}
