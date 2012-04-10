/*
            CVT - Computer Vision Tools Library

     Copyright (c) 2012, Philipp Heise, Sebastian Klose

    THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
    KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
    IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
    PARTICULAR PURPOSE.
 */
#include <cvt/vision/slam/stereo/KLTTracking.h>

namespace cvt
{
    KLTTracking::KLTTracking( size_t kltOctaves, float scaleFactor ) :
        _klt( 10 ),
        _numOctaves( kltOctaves ),
        _pyramid( _numOctaves, scaleFactor ),
        _ssdThreshold( Math::sqr( 60.0f ) ),
        _sadThreshold( 50 )
    {
    }

    void KLTTracking::trackFeatures( PointSet2d&			trackedPositions,
                                     std::vector<size_t>&		trackedFeatureIds,
                                     const std::vector<Vector2f>&	predictedPositions,
                                     const std::vector<size_t>&		predictedIds,
                                     const Image&			img )
    {
        _pyramid.update( img );

        SIMD* simd = SIMD::instance();
        const size_t nPixels = Math::sqr( PatchType::size() );
        const float  maxSSD = nPixels * _ssdThreshold;
        const size_t maxSAD = nPixels * _sadThreshold;

        Vector2f center;

        for( size_t i = 0; i < predictedPositions.size(); i++ ){
            const Vector2f& p = predictedPositions[ i ];

            //  get the patch
            PatchType* patch = _patchForId[ predictedIds[ i ] ];

            //  update patch position to current predicted one
            Eigen::Matrix3f & m = patch->pose().transformation();
            m( 0, 2 ) = p.x;
            m( 1, 2 ) = p.y;

            //  try to track the patch
            if( _klt.trackPatchMultiscale( *patch, _pyramid ) ){
                // successfully tracked: check SSD, SAD values
                float ssd = simd->SSD( patch->pixels(), patch->transformed(), nPixels );
                if( ssd < maxSSD ){
                    size_t sad = simd->SAD( patch->pixels(), patch->transformed(), nPixels );
                    if( sad < maxSAD ){
                        patch->currentCenter( center );
                        trackedPositions.add( Vector2d( center.x, center.y ) );
                        trackedFeatureIds.push_back( predictedIds[ i ] );
                    }
                }
            }
        }
    }

    void KLTTracking::addFeatureToDatabase( const Vector2f & f, size_t id )
    {
        if( _patchForId.size() <= id ){
            _patchForId.resize( id + 2000, 0 );
        } else if( _patchForId[ id ] != 0 ){
            delete _patchForId[ id ];
        }

        std::vector<PatchType*> patches;
        std::vector<Vector2f>  feature;
        feature.push_back( f );

        PatchType::extractPatches( patches, feature, _pyramid );

        if( patches.size() ){
            _patchForId[ id ] = patches[ 0 ];
        }
    }


    void KLTTracking::clear()
    {
        for( size_t i = 0; i < _patchForId.size(); i++ ){
            delete _patchForId[ i ];
        }
        _patchForId.clear();
    }
}
