#include "FeatureApp.h"

#include <cvt/gui/Application.h>

#include <cvt/gfx/GFX.h>

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
		_timerId = Application::registerTimer( 15, this );
		_window.setSize( 640, 480 );
		WidgetLayout wl;
		wl.setAnchoredTopBottom( 0, 0 );
		wl.setAnchoredLeftRight( 0, 0 );
		_window.addWidget( &_view, wl );
		_window.setVisible( true );
		_window.update();
		_detector = new FAST();
		//_detector = new Harris();
		_detector->setBorder( 30 );
		_oldset = NULL;
	}

	FeatureApp::~FeatureApp()
	{
		Application::unregisterTimer( _timerId );
	}


	void FeatureApp::onTimeout()
	{
		_video.nextFrame();
		_view.setImage( _video.frame() );

		FeatureSet featureset;

		_video.frame().convert( _gray, IFormat::GRAY_UINT8 );
		Time detectTime;
		_detector->detect( featureset, _gray );
		_avgDetectorTime += detectTime.elapsedMilliSeconds();
		featureset.filterNMS( 2 );

		BRIEF32* brief = new BRIEF32();
		brief->extract( _gray, featureset );

		_view.setFeatures( featureset, _gray.width(), _gray.height() );
		if( _oldset ) {
			std::vector<FeatureMatch> matches;
			brief->matchBruteForce( matches, *_oldset, 20.0f );
			_view.setTracks( matches, _gray.width(), _gray.height()  );
			delete _oldset;
		}
		_oldset = brief;

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
