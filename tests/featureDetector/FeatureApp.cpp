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
		_detector->setBorder( 45 );
		_oldset = NULL;
	}

	FeatureApp::~FeatureApp()
	{
		Application::unregisterTimer( _timerId );
	}


	void FeatureApp::onTimeout()
	{
		typedef ORB FeatureType;

		_video.nextFrame();
		_view.setImage( _video.frame() );

		ImagePyramid pyr( 4, 0.6f );

		FeatureSet featureset;

		_video.frame().convert( _gray, IFormat::GRAY_UINT8 );
		Time detectTime;
		pyr.update( _gray );
		_detector->detect( featureset, pyr );

		featureset.filterBest( 3000, true );
		featureset.filterNMS( 2, true );

		FeatureType* featuredesc = new FeatureType();
		featuredesc->extract( pyr, featureset );
		_avgDetectorTime += detectTime.elapsedMilliSeconds();

		_view.setFeatures( featureset, _gray.width(), _gray.height() );
		std::vector<FeatureMatch> matches;
		if( _oldset ) {
			featuredesc->matchBruteForce( matches, *_oldset, 35.0f );
			_view.setTracks( matches, _gray.width(), _gray.height()  );
			delete _oldset;
		}
		_oldset = featuredesc;

		_iter++;
		_allIter++;

		double t = _time.elapsedSeconds();
		if( t > 3.0 ){
			String title;
			title.sprintf( "Features - FPS %0.1f - Avg. Detection time: %0.1fms, Matches: %d" , _iter / t, _avgDetectorTime / _iter, matches.size() );
			_window.setTitle( title );
			_iter = 0;
			_avgDetectorTime = 0;
			_time.reset();
		}
	}
}
