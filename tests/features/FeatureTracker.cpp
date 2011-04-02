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
        _currentIntegralImg( 0 ),
		_threshold( 30 ),
        _searchRadius( 5 )
	{
		_featureDetector = new AGAST( OAST_9_16 );
		//_featureDetector = new AGAST( AGAST_7_12S );

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
							  std::vector<Feature2D> & features )
	{
		// try to refind features
		// get the current features
        if( _currentIntegralImg == 0 ){
            _currentIntegralImg = new IntegralImage( current, SUMMED_AREA | SQUARED_SUMMED_AREA );
        } else {
            _currentIntegralImg->update( current );
        }
        
        // try to refind the last features
        std::vector<Patch*>::iterator patchIt = _lastPatches.begin();
        std::vector<Patch*>::iterator patchOut = _lastPatches.begin();
        std::vector<Patch*>::const_iterator patchEnd = _lastPatches.end();
        std::vector<Feature2D>::iterator featIt = features.begin();
        std::vector<Feature2D>::iterator featStore = features.begin();
        
        while( patchIt != patchEnd ){
            if( matchFeatureNCC( current, *( *patchIt ), *featIt, *featStore ) > 0.9 ){
                // found good match
                featStore++;
                *patchOut = *patchIt;
                patchOut++;
            } else {
                delete *patchIt;
            }
            featIt++;
            patchIt++;
            featStore++;
        }
        features.erase( featStore, features.end() );
        _lastPatches.erase( patchOut, _lastPatches.end() );
        
        // detect new features if too few are present now
        size_t numFeatures = features.size();
        std::cout << "Tracked features: " << numFeatures << std::endl;
        if( numFeatures < 100 ){
            _featureDetector->extract( current, features );
            Recti roi( 0, 0, 7, 7 );
            while( numFeatures < features.size() ) {
                roi.x = features[ numFeatures ][ 0 ] - 3;
                roi.y = features[ numFeatures ][ 1 ] - 3;
                _lastPatches.push_back( new Patch( current, roi, NORMALIZE ) );
                numFeatures++;
            }
            //_featureDetector->extractMultiScale( current, features, 3 );
            
        }

	}
    
    float FeatureTracker::matchFeatureNCC( const Image & currentI, const Patch & patch, const Feature2D & startPos, Feature2D & out )
    {
        float bncc = 0;
        float ncc;
        
        Vector2i current, best;
        current.y = Math::max( 0, startPos.y - _searchRadius );
        
        int yBoundH = Math::min( startPos.y + _searchRadius + patch.height(),
                                 currentI.height() - patch.height() );
        int xBoundL = Math::max( startPos.x - _searchRadius, 0 );
        
        int xBoundH = Math::min( startPos.x + _searchRadius + patch.width(), 
                                 currentI.width() - patch.width() );
        
        while( current.y < yBoundH ){
            current.x = xBoundL;
            while( current.x < xBoundH ){
                ncc = _currentIntegralImg->ncc( currentI, patch, current );
                
                if( ncc > bncc ){
                    bncc = ncc;
                    best = current;
                }
                current.x++;
            }
            current.y++;
        }
        
        out = best;
        return bncc;
    }
}
