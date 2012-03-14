#ifndef FEATURE_TRACK_APP_H
#define FEATURE_TRACK_APP_H

#include <cvt/io/VideoInput.h>
#include <cvt/util/Time.h>

#include <cvt/vision/ORB.h>
#include <cvt/vision/FAST.h>

#include "GUI.h"

#include <vector>
#include <list>
#include <set>

namespace cvt
{
	class FASTFeatureApp : public TimeoutHandler
	{
		public:
			FASTFeatureApp( VideoInput * cam );
			~FASTFeatureApp();

			void setFastThreshold( uint8_t thresh ) { _fastDetector.setThreshold( thresh ); }
            void setNumScales( size_t v )           { _numScales = v; }
            void setScaleFactor( float v )          { _scaleFactor = v; }
            void setMaxFeatures( size_t v )         { _maxNumFeatures = v; }
            void setNonMaxSuppress( ToggleButton* p ) { _fastDetector.setNonMaxSuppress( p->state() ); }
		private:
			void onTimeout();

			VideoInput*	_cam;
			size_t		_numScales;
			float		_scaleFactor;
			size_t		_maxNumFeatures;
			bool		_nonMaxSuppress;
			uint32_t	_timerId;

			Gui			_gui;
			Time		_timer;
            size_t      _fpsIter;
            Image       _gray;

			FAST		_fastDetector;

	};

	inline FASTFeatureApp::FASTFeatureApp( VideoInput * cam ) :
		_cam( cam ),
		_numScales( 3 ),
		_scaleFactor( 0.5f ),
		_maxNumFeatures( 1000 ),
		_nonMaxSuppress( true ),
		_timerId( 0 ),
        _fpsIter( 0 )
	{
		// every 10 ms
		_timerId = Application::registerTimer( 5, this );

		Delegate<void ( uint8_t )> d( this, &FASTFeatureApp::setFastThreshold );
		_gui.observeFastThresholdSlider( d );
        Delegate<void ( size_t )> o( this, &FASTFeatureApp::setNumScales );
		_gui.observeOctaveSlider( o );
        Delegate<void ( float )> s( this, &FASTFeatureApp::setScaleFactor );
		_gui.observeScaleSlider( s );
        Delegate<void ( size_t )> n( this, &FASTFeatureApp::setMaxFeatures );
		_gui.observeMaxFeatureSlider( n );
        Delegate<void ( ToggleButton* )> nms( this, &FASTFeatureApp::setNonMaxSuppress );
		_gui.observeNonMaxSuppression( nms );

		_fastDetector.setThreshold( 25 );
		_fastDetector.setBorder( 16 );
    }

	inline FASTFeatureApp::~FASTFeatureApp()
	{
		Application::unregisterTimer( _timerId );
	}

	inline void FASTFeatureApp::onTimeout()
	{
		// get the next frame
        _cam->nextFrame();
        _cam->frame().convert( _gray, IFormat::GRAY_UINT8 );

		// detect features
		std::vector<Feature2Df> features;
		VectorFeature2DInserter<float> inserter( features );

		_fastDetector.extract( _gray, inserter );

		Image curr;
		_cam->frame().convert( curr, IFormat::RGBA_UINT8 );
        _gui.updateImage( curr, features );

        if( _fpsIter++ == 50 ){
            _gui.setFPS( 50000.0f / _timer.elapsedMilliSeconds() );
            _timer.reset();
            _fpsIter = 0;
        }

	}

}

#endif
