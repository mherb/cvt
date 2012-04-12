/*
            CVT - Computer Vision Tools Library

     Copyright (c) 2012, Philipp Heise, Sebastian Klose

    THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
    KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
    IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
    PARTICULAR PURPOSE.
 */
#ifndef CVT_DEPTHINITIALIZER_H
#define CVT_DEPTHINITIALIZER_H

#include <cvt/vision/CameraCalibration.h>
#include <cvt/vision/Vision.h>
#include <cvt/gfx/Image.h>
#include <cvt/math/Vector.h>
#include <cvt/util/Range.h>

#include <vector>

namespace cvt
{
    class DepthInitializer
    {
        public:
            struct DepthInitResult
            {
                Vector4f    point3d;
                Vector2f    meas0;
                Vector2f    meas1;
                float       reprojectionError;
            };

            DepthInitializer( const CameraCalibration& c0, const CameraCalibration& c1 );
            virtual ~DepthInitializer();

            /**
             *	\brief triangulate features from two (undistorted) views
             */
            virtual void triangulateFeatures( std::vector<DepthInitResult> & triangulated,
                                              const std::vector<Vector2f> & avoidPositionsImg0,
                                              const Image& view0, const Image& view1 ) = 0;

            void  setMaxTriangulationError( float v ) { _maxTriangError = v; }
            float maxTriangulationError() const       { return _maxTriangError; }

            void setAllowedDepthRange( const Rangef & range ) { _depthRange = range; }
            const Rangef allowedDepthRange() const { return _depthRange; }

            const CameraCalibration& calibration0() const { return _calib0; }
            const CameraCalibration& calibration1() const { return _calib1; }

        protected:
            const CameraCalibration & _calib0;
            const CameraCalibration & _calib1;

            Rangef                    _depthRange;
            float                     _maxTriangError;

            /* triangulate a new point */
            void triangulateSinglePoint( DepthInitResult&   result,
                                         const Matrix4f&    projMat0,
                                         const Matrix4f&	projMat1 ) const;

    };

    inline DepthInitializer::DepthInitializer( const CameraCalibration& c0, const CameraCalibration& c1 ) :
        _calib0( c0 ),
        _calib1( c1 ),
        _depthRange( 0.1f, 50.0f ),
        _maxTriangError( 3.0f )
    {
    }

   inline DepthInitializer::~DepthInitializer()
   {
   }

   inline void DepthInitializer::triangulateSinglePoint( DepthInitResult& result,
                                                         const Matrix4f&  projMat0,
                                                         const Matrix4f&  projMat1 ) const
   {
       Vector4f repr;
       Vector2f repr2;

       Vision::triangulate( result.point3d, projMat0, projMat1, result.meas0, result.meas1 );

       // normalize 4th coord;
       result.point3d /= result.point3d.w;
       result.reprojectionError = Math::MAXF;
       if( result.point3d.z > _depthRange.min && result.point3d.z < _depthRange.max ){
           repr = projMat0 * result.point3d;
           repr2.x = repr.x / repr.z;
           repr2.y = repr.y / repr.z;
           result.reprojectionError = ( result.meas0 - repr2 ).length();

           repr = projMat1 * result.point3d;
           repr2.x = repr.x / repr.z;
           repr2.y = repr.y / repr.z;
           result.reprojectionError += ( result.meas1 - repr2 ).length();

           result.reprojectionError /= 2.0f;
       }
   }
}

#endif
