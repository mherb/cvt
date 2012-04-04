#ifndef FEATURE_TRACK_APP_H
#define FEATURE_TRACK_APP_H

#include <cvt/io/VideoInput.h>
#include <cvt/util/Time.h>

#include <cvt/vision/ORB.h>

#include "GUI.h"
#include "FASTFeatureTracking.h"

#include <vector>
#include <list>
#include <set>

namespace cvt
{

	/**
	 *	This is the controller of the app
	 */
	class FeatureTrackApp : public TimeoutHandler
	{
		public:
			FeatureTrackApp( VideoInput * cam );
			~FeatureTrackApp();

			void setFastThreshold( uint8_t thresh ) { _featureTracking.setFASTThreshold( thresh ); }
            void setNumScales( size_t v )           { _featureTracking.setNumOctaves( v ); }
            void setScaleFactor( float v )          { _featureTracking.setScaleFactor( v ); }
            void setNonMaxSuppress( ToggleButton* p ) { _featureTracking.setNonMaxSuppression( p->state() ); }
            void setMatchRadius( size_t v )         { _featureTracking.setMaxMatchingRadius( v ); }
            void setMaxFeatures( size_t v )         { _maxNumFeatures = v; }
			void setGridSize( size_t v )			{ _gridSize = v; };

		private:
			void onTimeout();
            
			VideoInput*				_cam;
			FASTFeatureTracking		_featureTracking;
			size_t					_maxNumFeatures;
			size_t					_gridSize;
			
			uint32_t				_timerId;

			Gui						_gui;
			Time					_timer;
            size_t      			_fpsIter;
            Image       			_gray;

			std::vector<FASTFeatureTracking::PatchType*>	_features;

			void addNewPatches();
			void drawResult();
	};

	inline FeatureTrackApp::FeatureTrackApp( VideoInput * cam ) :
		_cam( cam ),
		_maxNumFeatures( 200 ),
		_gridSize( 20 ),
		_timerId( 0 ),
        _fpsIter( 0 )
	{
		// every 10 ms
		_timerId = Application::registerTimer( 1, this );

		Delegate<void ( float )> a( &_featureTracking, &FASTFeatureTracking::setKLTSSDThreshold );
		_gui.observeKLTSSDSlider( a );
		
		Delegate<void ( size_t )> grid( this, &FeatureTrackApp::setGridSize );
		_gui.observeGridSizeSlider( grid );

		Delegate<void ( uint8_t )> d( this, &FeatureTrackApp::setFastThreshold );
		_gui.observeFastThresholdSlider( d );

		Delegate<void ( float )> fastsad( &_featureTracking, &FASTFeatureTracking::setFASTSADThreshold );
		_gui.observeFastSADThresholdSlider( fastsad );

		Delegate<void ( size_t )> o( this, &FeatureTrackApp::setNumScales );
		_gui.observeOctaveSlider( o );

		Delegate<void ( float )> s( this, &FeatureTrackApp::setScaleFactor );
		_gui.observeScaleSlider( s );

		Delegate<void ( size_t )> n( this, &FeatureTrackApp::setMaxFeatures );
		_gui.observeMaxFeatureSlider( n );

		Delegate<void ( size_t )> mr( this, &FeatureTrackApp::setMatchRadius );
		_gui.observeMatchRadiusSlider( mr );

		Delegate<void ( ToggleButton* )> nms( this, &FeatureTrackApp::setNonMaxSuppress );
		_gui.observeNonMaxSuppression( nms );
    }

	inline FeatureTrackApp::~FeatureTrackApp()
	{
		Application::unregisterTimer( _timerId );
	}

	inline void FeatureTrackApp::onTimeout()
	{
		// get the next frame
        _cam->nextFrame();
        _cam->frame().convert( _gray, IFormat::GRAY_UINT8 );

		// try to track the features:
		std::vector<FASTFeatureTracking::PatchType*> tracked;
		std::vector<FASTFeatureTracking::PatchType*> lost;
	   _featureTracking.trackFeatures( tracked, lost, _features, _gray );

	   for( size_t l = 0; l < lost.size(); l++ )
		   delete lost[ l ];

	   _features = tracked;

	   if( tracked.size() < 40 ){
		   std::cout << "NEED MORE FEATURES:" << std::endl;
		   addNewPatches();
	   }

	   drawResult();

        // if number of tracked features drops below thresh,
        // add the unmachted from this frame

        if( _fpsIter++ == 50 ){
            _gui.setFPS( 50000.0f / _timer.elapsedMilliSeconds() );
            _timer.reset();
            _fpsIter = 0;
        }

	}

	void FeatureTrackApp::addNewPatches()
	{
		const std::vector<Feature2Df>& detected   = _featureTracking.lastDetectedFeatures();
		const std::set<size_t>& alreadyAssociated = _featureTracking.associatedFeatures();
		const ImagePyramid&		pyramid			  = _featureTracking.pyramid();

		// apply grid filtering: 
		FeatureFilter gridfilter( _gridSize, pyramid[ 0 ].width(), pyramid[ 0 ].height() );


		const std::set<size_t>::const_iterator assocEnd = alreadyAssociated.end();
		for( size_t i = 0; i < detected.size(); i++ ){
			if( assocEnd == alreadyAssociated.find( i ) ){
				// free feature:
				gridfilter.addFeature( &detected[ i ] );
			}
		}
		
		std::vector<const Feature2Df*> filteredFeatures;
		gridfilter.gridFilteredFeatures( filteredFeatures, _maxNumFeatures );
		
		std::vector<Vector2f> freeFeatures;
		freeFeatures.resize( filteredFeatures.size() );
		for( size_t i = 0; i < filteredFeatures.size(); i++ ){
			freeFeatures[ i ] = filteredFeatures[ i ]->pt;
		}

		FASTFeatureTracking::PatchType::extractPatches( _features, freeFeatures, pyramid );
	}

	void FeatureTrackApp::drawResult()
	{
		Image curr;
		_cam->frame().convert( curr, IFormat::RGBA_UINT8 );
		
		// copy all feature positions:
		std::vector<Vector2f> tracked;
		tracked.resize( _features.size() );
		for( size_t i = 0; i < _features.size(); i++ ){
			_features[ i ]->currentCenter( tracked[ i ] );
		}

		_gui.updateImage( curr, tracked );
	}

}

#endif
