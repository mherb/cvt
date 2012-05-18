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
        _klt( 5 ),
        _numOctaves( kltOctaves ),
        _pyramid( _numOctaves, scaleFactor ),
        _ssdThreshold( Math::sqr( 20.0f ) ),
        _sadThreshold( 20 )
    {
    }

    KLTTracking::~KLTTracking()
    {
        clear();
    }

    void KLTTracking::trackFeatures( PointSet2d&                    trackedPositions,
                                     std::vector<size_t>&           trackedFeatureIds,
                                     const std::vector<Vector2f>&	predictedPositions,
                                     const std::vector<size_t>&		predictedIds,
                                     const Image&                   img )
    {
        _pyramid.update( img );

        SIMD* simd = SIMD::instance();
        const size_t nPixels = Math::sqr( PatchType::size() );
        const float  maxSSD = nPixels * _ssdThreshold;
        const size_t maxSAD = nPixels * _sadThreshold;

        Vector2f center;

        for( size_t i = 0; i < predictedPositions.size(); i++ ){
            const Vector2f& p = predictedPositions[ i ];

            size_t id = predictedIds[ i ];
            //  get the patch
            PatchType* patch = _patchForId[ id ];

            if( patch == 0 ){
                // this was a bad PATCH
                continue;
            }

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
                        trackedFeatureIds.push_back( id );
                    }
                }
            }
        }
    }

    void KLTTracking::addFeatureToDatabase( const Vector2f & f, size_t id )
    {
        std::vector<PatchType*> patches;
        std::vector<Vector2f>  feature;
        feature.push_back( f );

        PatchType::extractPatches( patches, feature, _pyramid );

        if( id != _patchForId.size() ){
            throw CVTException( "Patch IDs out of sync" );
        }

        // FIXME: shall we handle this differently?
        // Problem: Map has already added feature with id at this point
        if( patches.size() )
            _patchForId.push_back( patches[ 0 ] );
        else
            _patchForId.push_back( 0 );

    }


    void KLTTracking::clear()
    {
        for( size_t i = 0; i < _patchForId.size(); i++ ){
            delete _patchForId[ i ];
        }
        _patchForId.clear();
    }
}
