#include "FeatureTracker.h"

#include <cvt/geom/KDTree.h>
#include <cvt/vision/AGAST.h>
#include <cvt/vision/FAST.h>
#include <cvt/math/Math.h>

#include <cvt/util/RNG.h>
#include <cvt/io/ImageIO.h>

#include <set>

namespace cvt {

	FeatureTracker::FeatureTracker() :
		_featureDetector( 0 ),
        _lastBrighter( 0 ),
        _lastDarker( 0 ),
        _numLast( 0 ),
        _threshold( 35 )
	{
		//_featureDetector = new AGAST( OAST_9_16 );
        _featureDetector = new AGAST( AGAST_7_12S );
        
        ( ( AGAST* )_featureDetector )->setNonMaxSuppress( true );
        ( ( AGAST* )_featureDetector )->setThreshold( _threshold );
        
        
        
		//_featureDetector = new FAST( SEGMENT_9 );
        //( ( FAST* )_featureDetector )->setNonMaxSuppress( true );
        //( ( FAST* )_featureDetector )->setThreshold( _threshold );
        //( ( FAST* )_featureDetector )->setMinScore( 10 );
        
        
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
		//_featureDetector->extractMultiScale( current, newFeatures, 3 );
        
        /*size_t stride;
        const uint8_t * p = current.map( &stride );
        
        SegmentRing test;        
        bool correct = true;
        for( size_t i = 0; i < newFeatures.size(); i++ ){
            const uint8_t * center = p + newFeatures[ i ].y * stride + newFeatures[ i ].x;
            bool current = test.update( center, stride, _threshold );
            if( !current ){
                std::cout << "BAD FEATURE: " << newFeatures[ i ] << std::endl;
            }
            correct &= current;
        }                
        current.unmap( p );
        
        if( !correct ){
            ImageIO::savePNG( current, "fasttest.png" );
            exit( 0 );
        }*/
	}
	
}
