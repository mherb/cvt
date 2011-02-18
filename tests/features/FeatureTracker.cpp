#include "FeatureTracker.h"

#include <cvt/geom/KDTree.h>
#include <cvt/vision/AGAST.h>
#include <cvt/vision/FAST.h>
#include <cvt/math/Math.h>

#include <cvt/util/RNG.h>

#include <set>

namespace cvt {

	FeatureTracker::FeatureTracker() :
		_featureDetector( 0 ),
        _lastBrighter( 0 ),
        _lastDarker( 0 ),
        _numLast( 0 )
	{
		//_featureDetector = new AGAST( OAST_9_16 );
        //( ( AGAST* )_featureDetector )->setNonMaxSuppress( false );
        
		_featureDetector = new FAST( SEGMENT_9 );
        ( ( FAST* )_featureDetector )->setNonMaxSuppress( true );
        ( ( FAST* )_featureDetector )->setThreshold( 25 );
        ( ( FAST* )_featureDetector )->setMinScore( 25 );
        
        RNG rng( time( NULL ) );
	}

	FeatureTracker::~FeatureTracker()
	{
		if( _featureDetector )
			delete _featureDetector;
	}

	void FeatureTracker::run( const Image & current,							  
							  std::vector<Feature2D> & newFeatures )
	{
        // get the current features
		_featureDetector->extract( current, newFeatures );        
		//_featureDetector->extractMultiScale( current, newFeatures, 2 );
        
        if( _numLast ){
            // try to find the best matching features in the set!
        }
	}
	
}
