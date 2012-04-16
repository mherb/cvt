/*
            CVT - Computer Vision Tools Library

     Copyright (c) 2012, Philipp Heise, Sebastian Klose

    THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
    KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
    IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
    PARTICULAR PURPOSE.
 */

#include <cvt/vision/slam/stereo/PatchStereoInit.h>
#include <cvt/vision/FeatureFilter.h>
#include <cvt/gfx/IMapScoped.h>
#include <cvt/geom/Line2D.h>

#include <cvt/util/ParamInfo.h>

#include <set>

namespace cvt {

    static ParamInfo* _pinfos[] = {
        new ParamInfoTyped<size_t>( "maxNewFeatures", 0, 5000, 400, true, 1, offsetof( PatchStereoInit::Parameters, maxNewFeatures ) ),
        new ParamInfoTyped<size_t>( "maxSAD", 0, 255, 30, true, 1, offsetof( PatchStereoInit::Parameters, maxSAD ) ),
        new ParamInfoTyped<float>( "maxEpilineDistance", 0.0f, 10.0f, 1.0f, true, 1, offsetof( PatchStereoInit::Parameters, maxEpilineDistance ) ),
        new ParamInfoTyped<float>( "maxReprojectionError", 0.0f, 10.0f, 3.0f, true, 1, offsetof( PatchStereoInit::Parameters, maxReprojectionError ) ),
        new ParamInfoTyped<float>( "minDepth", 0.1f, 5.0f, 0.5f, true, 1, offsetof( PatchStereoInit::Parameters, minDepth ) ),
        new ParamInfoTyped<float>( "maxDepth", 3.0f, 100.0f, 30.0f, true, 1, offsetof( PatchStereoInit::Parameters, maxDepth ) ),
        new ParamInfoTyped<uint8_t>( "fastThreshold", 1, 255, 20, true, 1, offsetof( PatchStereoInit::Parameters, fastThreshold ) )
    };

    PatchStereoInit::PatchStereoInit( const CameraCalibration& c0, const CameraCalibration& c1 ):
        DepthInitializer( c0, c1 ),
        _pyramidView0( 3, 0.5f ),
        _pyramidView1( 3, 0.5f ),
        _pset( _pinfos, 7, false )
    {
        _params = _pset.ptr<Parameters>();
        _detector.setBorder( 16 );
        _detector.setThreshold( _params->fastThreshold );
        _detector.setNonMaxSuppress( true );

        // calc the fundamental matrix from the calibration data
        Vision::composeFundamental( _fundamental,
                                    c0.intrinsics(), c0.extrinsics(),
                                    c1.intrinsics(), c1.extrinsics() );
        _simd = SIMD::instance();
    }

    PatchStereoInit::~PatchStereoInit()
    {
        _simd = 0;
    }

    void PatchStereoInit::triangulateFeatures( std::vector<DepthInitResult> & triangulated,
                                               const std::vector<Vector2f> & avoidPositionsImg0,
                                               const Image& view0, const Image& view1 )
    {
        _pyramidView0.update( view0 );
        _pyramidView1.update( view1 );

        std::vector<Feature2Df> features0, features1;

        // first, detect the features in both views over multiple scales
        _detector.setNonMaxSuppress( true );
        detectFeatures( features0, _pyramidView0 );
        _detector.setNonMaxSuppress( false );
        detectFeatures( features1, _pyramidView1 );

        std::vector<Vector2f> filtered0;
        filterFeatures( filtered0, features0, avoidPositionsImg0 );

        // try to find matches using the epipolar constraint
        // and SAD matching based on the image data
        matchAndTriangulate( triangulated, filtered0, features1 );
    }

    void PatchStereoInit::detectFeatures( std::vector<Feature2Df>& features, const ImagePyramid& pyramid )
    {
        // detect for each scale
        ScaleSpaceFeatureInserterf inserter( features );
        inserter.scale  = 1.0f;
        for( size_t i = 0; i < pyramid.octaves(); i++ ){
            inserter.octave = i;
            _detector.extract( pyramid[ i ], inserter );
            inserter.scale *= pyramid.scaleFactor();
        }
    }

    void PatchStereoInit::filterFeatures( std::vector<Vector2f>& filtered,
                                          const std::vector<Feature2Df>& features,
                                          const std::vector<Vector2f>& avoid )
    {
        // perform a grid filtering on the first view to have better distribution
        FeatureFilter gridFilter( 30, _pyramidView0[ 0 ].width(), _pyramidView0[ 0 ].height() );

        for( size_t i = 0; i < features.size(); i++ ){
            // check if there is a too close feature in the avoid set
            const Feature2Df & f = features[ i ];
            bool take = true;
            for( size_t k = 0; k < avoid.size(); k++ ){
                if( ( f.pt - avoid[ k ] ).lengthSqr() > 400.0f ){
                    take = false;
                    break;
                }
            }

            if( take ){
                gridFilter.addFeature( &features[ i ] );
            }
        }

        // now filter the features
        gridFilter.gridFilteredFeatures( filtered, _params->maxNewFeatures );
    }

    void PatchStereoInit::matchAndTriangulate( std::vector<DepthInitResult>& triangulated,
                                               const std::vector<Vector2f>& f0,
                                               const std::vector<Feature2Df>& f1 )
    {
        Vector3f p0;
        p0.z = 1.0f;

        std::set<size_t> assigned;

        IMapScoped<const uint8_t> map0( _pyramidView0[ 0 ] );
        IMapScoped<const uint8_t> map1( _pyramidView0[ 1 ] );
        const size_t PatchHalf = PatchSize >> 1;
        size_t maxSAD = _params->maxSAD * Math::sqr( PatchSize );

        Rangef depthRange( _params->minDepth, _params->maxDepth );

        size_t w0 = _pyramidView0[ 0 ].width();
        size_t h0 = _pyramidView0[ 0 ].height();
        size_t w1 = _pyramidView1[ 0 ].width();
        size_t h1 = _pyramidView1[ 0 ].height();

        for( size_t i = 0; i < f0.size(); i++ ){
            const Vector2f& pos0 = f0[ i ];

            p0.x = pos0.x;
            p0.y = pos0.y;

            if( p0.x < PatchHalf || ( p0.x + PatchHalf ) >= w0 ||
                p0.y < PatchHalf || ( p0.y + PatchHalf ) >= h0 )
                continue;

            const uint8_t* ptr0 = map0.ptr() + (int)( p0.y - PatchHalf ) * map0.stride() + (int)p0.x - PatchHalf;

            Line2Df l( _fundamental * p0 );

            const std::set<size_t>::const_iterator assignedEnd = assigned.end();
            size_t bestSAD = maxSAD;
            size_t bestIdx =  0;

            for( size_t k = 0; k < f1.size(); k++ ){
                if( assigned.find( k ) == assignedEnd ){
                    const Vector2f& p1 = f1[ k ].pt;

                    if( p1.x < PatchHalf || ( p1.x + PatchHalf ) >= w1 ||
                        p1.y < PatchHalf || ( p1.y + PatchHalf ) >= h1 )
                        continue;

                    float d = Math::abs( l.distance( p1 ) );
                    if( d < _params->maxEpilineDistance ){
                        const uint8_t* ptr1 = map1.ptr() + (int)( p1.y - PatchHalf ) * map1.stride() + (int)p1.x - PatchHalf;
                        // check if SAD is smaller than current best
                        size_t sad = computePatchSAD( ptr0, map0.stride(), ptr1, map1.stride() );

                        if( sad < bestSAD ){
                            bestSAD = sad;
                            bestIdx = k;
                        }
                    }
                }
            }

            if( bestSAD < maxSAD ){
                // found a match:
                assigned.insert( bestIdx );
                // TODO:do a subpixel refinement of the patchpos in the second image

                // triangulate
                DepthInitResult result;
                result.meas0 = pos0;
                result.meas1 = f1[ bestIdx ].pt;
                Vision::correctCorrespondencesSampson( result.meas0, result.meas1, _fundamental );

                triangulateSinglePoint( result,
                                        _calib0.projectionMatrix(),
                                        _calib1.projectionMatrix(),
                                        depthRange );

                if( result.reprojectionError < _params->maxReprojectionError ){
                    triangulated.push_back( result );
                }
            }
        }
    }

    size_t PatchStereoInit::computePatchSAD( const uint8_t* p0, size_t s0,
                                             const uint8_t* p1, size_t s1 ) const
    {
        size_t n = PatchSize;
        size_t ret = 0;

        while( n-- ){
            ret += _simd->SAD( p0, p1, PatchSize );
            p0 += s0;
            p1 += s1;
        }

        return ret;
    }


}
