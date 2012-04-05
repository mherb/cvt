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

            void setAllowedDepthRange( const Rangef & range ) { _depthRange = range; }
            const Rangef allowedDepthRange() const { return _depthRange; }

            const CameraCalibration& calibration0() const { return _calib0; }
            const CameraCalibration& calibration1() const { return _calib1; }

        protected:
            const CameraCalibration & _calib0;
            const CameraCalibration & _calib1;

            Rangef                    _depthRange;

            /* triangulate a new point */
            float triangulateSinglePoint( Vector4f&			newPoint,
                                          const Vector2f&	point0,
                                          const Vector2f&	point1,
                                          const Matrix4f&	projMat0,
                                          const Matrix4f&	projMat1 ) const;

    };

    inline DepthInitializer::DepthInitializer( const CameraCalibration& c0, const CameraCalibration& c1 ) :
        _calib0( c0 ),
        _calib1( c1 ),
        _depthRange( 0.1f, 50.0f )
    {
    }

   inline DepthInitializer::~DepthInitializer()
   {
   }

   inline float DepthInitializer::triangulateSinglePoint( Vector4f&         newPoint,
                                                          const Vector2f&   point0, const Vector2f& 	point1,
                                                          const Matrix4f& 	projMat0, const Matrix4f& 	projMat1 ) const
   {
       Vector4f repr;
       Vector2f repr2;

       Vision::triangulate( newPoint, projMat0, projMat1, point0, point1 );

       // normalize 4th coord;
       newPoint /= newPoint.w;
       float error = 10000.0f;
       if( newPoint.z > _depthRange.min && newPoint.z < _depthRange.max ){
           repr = projMat0 * newPoint;
           repr2.x = repr.x / repr.z;
           repr2.y = repr.y / repr.z;
           error = ( point0 - repr2 ).length();

           repr = projMat1 * newPoint;
           repr2.x = repr.x / repr.z;
           repr2.y = repr.y / repr.z;
           error += ( point1 - repr2 ).length();

           error /= 2.0f;
       }

       return error;
   }
}

#endif
