#ifndef FEATURE_TRACK_APP_H
#define FEATURE_TRACK_APP_H

#include <cvt/io/VideoInput.h>
#include <cvt/util/Time.h>

#include <cvt/vision/ORB.h>

#include "GUI.h"

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

			void setFastThreshold( uint8_t thresh ) { _fastThreshold = thresh; }
            void setNumScales( size_t v )           { _numScales = v; }
            void setScaleFactor( float v )          { _scaleFactor = v; }
            void setMaxFeatures( size_t v )         { _maxNumFeatures = v; }
            void setNonMaxSuppress( ToggleButton* p ) { _nonMaxSuppress = p->state(); }
            void setMaxDescDist( size_t v )         { _maxDescDistance = v; }
            void setMatchRadius( size_t v )         { _windowRadius = v; }

            // match the ORB features of the current image, against the last tracked ones
            // lost will contain those from the last step that could not be found again
            // unmatched will contain the features of the image, that are "new"
            void trackFeatures( const ORB & current,
                                std::vector<ORBFeature> & lost,
                                std::vector<ORBFeature> & unmatched );

		private:
			void onTimeout();
            int matchInWindow( const ORBFeature & f, const ORB & orb ) const;
            void addNewFeatures( const std::vector<ORBFeature> & features );

			VideoInput*	_cam;
			size_t		_numScales;
			float		_scaleFactor;
			uint8_t		_fastThreshold;
			size_t		_maxNumFeatures;
			bool		_nonMaxSuppress;
			uint32_t	_timerId;
            float       _windowRadius;
            size_t      _maxDescDistance;

			Gui			_gui;
			Time		_timer;
            size_t      _fpsIter;
            Image       _gray;

            std::list<ORBFeature> _tracked;
	};

	inline FeatureTrackApp::FeatureTrackApp( VideoInput * cam ) :
		_cam( cam ),
		_numScales( 3 ),
		_scaleFactor( 0.5f ),
		_fastThreshold( 25 ),
		_maxNumFeatures( 1000 ),
		_nonMaxSuppress( true ),
		_timerId( 0 ),
        _windowRadius( 30.0f ),
        _maxDescDistance( 50 ),
        _fpsIter( 0 )
	{
		// every 10 ms
		_timerId = Application::registerTimer( 1, this );

		Delegate<void ( uint8_t )> d( this, &FeatureTrackApp::setFastThreshold );
		_gui.observeFastThresholdSlider( d );
        Delegate<void ( size_t )> o( this, &FeatureTrackApp::setNumScales );
		_gui.observeOctaveSlider( o );
        Delegate<void ( float )> s( this, &FeatureTrackApp::setScaleFactor );
		_gui.observeScaleSlider( s );
        Delegate<void ( size_t )> n( this, &FeatureTrackApp::setMaxFeatures );
		_gui.observeMaxFeatureSlider( n );
        Delegate<void ( size_t )> dd( this, &FeatureTrackApp::setMaxDescDist );
		_gui.observeMaxDescDistanceSlider( dd );
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

		// compute the features for the current frame
        ORB orb( _gray, _numScales, _scaleFactor, _fastThreshold, _maxNumFeatures, _nonMaxSuppress );

        // try to find the features from the last frame
        std::vector<ORBFeature> lostFeatures, unMatched;

        trackFeatures( orb, lostFeatures, unMatched );

		Image curr;
		_cam->frame().convert( curr, IFormat::RGBA_UINT8 );
        _gui.updateImage( curr, _tracked, lostFeatures, unMatched );

        // if number of tracked features drops below thresh,
        // add the unmachted from this frame
        if( _tracked.size() < 100 ){
            addNewFeatures( unMatched );
        }

        if( _fpsIter++ == 50 ){
            _gui.setFPS( 50000.0f / _timer.elapsedMilliSeconds() );
            _timer.reset();
            _fpsIter = 0;
        }

	}

    inline void FeatureTrackApp::trackFeatures( const ORB & current,
                                                std::vector<ORBFeature> & lost,
                                                std::vector<ORBFeature> & unmatched )
    {
        // we want to find the best matching orb feature from current, that lies
        // within a certain distance from the "predicted" position
        std::list<ORBFeature>::iterator predicted = _tracked.begin();
        std::list<ORBFeature>::iterator delIter;
        const std::list<ORBFeature>::iterator tEnd = _tracked.end();

        std::set<int> matchedIndices;

        while( predicted != tEnd ){
            int matchIdx = matchInWindow( *predicted, current );

            if( matchIdx != -1 ){
                // TODO: avoid double matching of a single ORB feature
                *predicted = current[ matchIdx ];
                predicted++;

                matchedIndices.insert( matchIdx );
            } else {
                // not found, remove from tracked and add to lost features
                lost.push_back( *predicted );
                delIter = predicted;
                predicted++;
                _tracked.erase( delIter );
            }
        }

        const std::set<int>::iterator matchedEnd = matchedIndices.end();

        for( size_t i = 0; i < current.size(); i++ ){
            if( matchedIndices.find( i ) == matchedEnd ){
                unmatched.push_back( current[ i ] );
            }
        }
    }

    inline int FeatureTrackApp::matchInWindow( const ORBFeature & f, const ORB & orb ) const
    {
        int idx = -1;
        size_t bestDistance = _maxDescDistance;
        size_t currDist;

        for( size_t i = 0; i < orb.size(); i++ ){
            if( ( f.pt - orb[ i ].pt ).length() < _windowRadius ){
                // try to match
                currDist = f.distance( orb[ i ] );
                if( currDist < bestDistance ){
                    bestDistance = currDist;
                    idx = i;
                }
            }
        }

        return idx;
    }

    inline void FeatureTrackApp::addNewFeatures( const std::vector<ORBFeature> & features )
    {
        for( size_t i = 0; i < features.size(); i++ ){
            _tracked.push_back( features[ i ] );
        }
    }

}

#endif
