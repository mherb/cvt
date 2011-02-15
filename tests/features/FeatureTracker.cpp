#include "FeatureTracker.h"

#include <cvt/geom/KDTree.h>
#include <cvt/vision/AGAST.h>
#include <cvt/vision/FAST.h>
#include <cvt/math/Math.h>

#include <cvt/util/RNG.h>

#include <set>

namespace cvt {

	FeatureTracker::FeatureTracker() :
		_featureDetector( 0 )
	{
		//_featureDetector = new AGAST( OAST_9_16 );
        //( ( AGAST* )_featureDetector )->setNonMaxSuppress( false );
        
		_featureDetector = new FAST( SEGMENT_9 );
        ( ( FAST* )_featureDetector )->setNonMaxSuppress( true );
        
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
		_featureDetector->extract( current, newFeatures );
		//_featureDetector->extractMultiScale( current, newFeatures, 2 );
	}
	
}
