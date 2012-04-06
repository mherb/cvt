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
    KLTTracking::KLTTracking( const CameraCalibration& c0, const CameraCalibration& c1 ) :
        _camCalib0( c0 ),
        _camCalib1( c1 ),
        _stereoMatcher( 5.0f/*maxLineDist*/, 60 /*maxDescdist*/, c0, c1 ),
        _maxTriangReprojError( 7.0f ),
        _klt( 10 )
    {
    }

    void KLTTracking::trackFeatures( PointSet2d&						trackedPositions,
                                     std::vector<size_t>&			trackedFeatureIds,
                                     const std::vector<Vector2f>&	predictedPositions,
                                     const std::vector<size_t>&		predictedIds,
                                     const Image&					img )
    {
        throw CVTException( "IMPLEMENT ME" );
    }

    void KLTTracking::addFeatureToDatabase( const Vector2f & f, size_t id )
    {
        throw CVTException( "IMPLEMENT ME" );
    }


    void KLTTracking::clear()
    {
        for( size_t i = 0; i < _patchForId.size(); i++ ){
            delete _patchForId[ i ];
        }
        _patchForId.clear();
    }
}
