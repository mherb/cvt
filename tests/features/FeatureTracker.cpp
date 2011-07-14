#include "FeatureTracker.h"

#include <cvt/geom/KDTree.h>
#include <cvt/vision/AGAST.h>
#include <cvt/vision/FAST.h>
#include <cvt/math/Math.h>

#include <cvt/util/RNG.h>

#include <set>

namespace cvt {
	FeatureTracker::FeatureTracker() :
		_featureDetector( SEGMENT_9 ),
        _currentIntegralImg( 0 ),
		_threshold( 35 ),
		_nccThreshold( 0.8 ),
        _searchRadius( 5 )
	{
        _featureDetector.setNonMaxSuppress( false );
		_featureDetector.setThreshold( _threshold );
		_featureDetector.setMinScore( 20 );
	}

	FeatureTracker::~FeatureTracker()
	{
        if( _currentIntegralImg )
            delete _currentIntegralImg;
	}


	void FeatureTracker::setCornerThreshold( int thresh )
	{
		_threshold = thresh;
        _featureDetector.setThreshold( _threshold );
		std::cout << "New Corner Threshold: " << _threshold << std::endl;
	}

	void FeatureTracker::run( const Image & current,
							  std::vector<Feature2Df> & features )
	{
        /*
        if( _currentIntegralImg == 0 ){
            _currentIntegralImg = new IntegralImage( current, SUMMED_AREA | SQUARED_SUMMED_AREA );
        } else {
            _currentIntegralImg->update( current );
        }*/


        features.clear();

        /*
        std::vector<Feature2Df>::iterator lastF = _lastFeatures.begin();
        std::vector<Feature2Df>::iterator featureEnd = _lastFeatures.end();
        std::vector<Patch*>::iterator patchIt = _lastPatches.begin();

        std::vector<Patch*> trackedPatches;

        Feature2Df tmp;

        while( lastF != featureEnd ){
            double ncc = matchFeatureNCC( current, *( *patchIt ), *lastF, tmp );
            if( ncc > _nccThreshold ){
                // tmp is position
                features.push_back( tmp );
                trackedPatches.push_back( *patchIt );
            } else {
                delete *patchIt;
            }

            lastF++;
            patchIt++;
        }

        _lastFeatures.clear();
        _lastFeatures.resize( trackedPatches.size() );
        std::copy( features.begin(), features.end(), _lastFeatures.begin() );

        _lastPatches.clear();
        _lastPatches.resize( features.size() );
        std::copy( trackedPatches.begin(), trackedPatches.end(), _lastPatches.begin() );
*/
        // detect new features if too few are present now
        // features.clear();
        size_t numFeatures = features.size();
  //      std::cout << "Tracked features: " << numFeatures << std::endl;
        if( numFeatures < 100 ){
            _featureDetector.extract( current, features );
/*
            Recti roi( 0, 0, 21, 21 );
            while( numFeatures < features.size() ) {
                _lastFeatures.push_back( features[ numFeatures ] );
                roi.x = features[ numFeatures ][ 0 ] - 3;
                roi.y = features[ numFeatures ][ 1 ] - 3;
                _lastPatches.push_back( new Patch( current, roi, NORMALIZE ) );
                numFeatures++;
            }
 */
        }

	}

    float FeatureTracker::matchFeatureNCC( const Image & currentI, const Patch & patch, const Vector2f & startPos, Vector2f & out )
    {
        float bncc = 0;
        float ncc;

        Vector2i current, best;
		best.x = startPos.x;
        best.y = startPos.y;

        current.y = Math::max<float>( 0, startPos.y - _searchRadius );

        int yBoundH = Math::min<float>( startPos.y + _searchRadius + patch.height(), currentI.height() - patch.height() );
        int xBoundL = Math::max<float>( startPos.x - _searchRadius, 0 );

        int xBoundH = Math::min<float>( startPos.x + _searchRadius + patch.width(), currentI.width() - patch.width() );

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

        out.x = best.x;
        out.y = best.y;

        return bncc;
    }
}
