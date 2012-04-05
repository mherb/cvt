/*
         CVT - Computer Vision Tools Library

    Copyright (c) 2012, Philipp Heise, Sebastian Klose

   THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
   KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
   IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
   PARTICULAR PURPOSE.
 */
#ifndef CVT_FEATURE_TRACKING_H
#define CVT_FEATURE_TRACKING_H

#include <vector>
#include <cvt/geom/PointSet.h>
#include <cvt/util/Signal.h>
#include <cvt/gfx/Image.h>

namespace cvt
{
   class FeatureTracking
   {
      public:
         virtual ~FeatureTracking() {}

         /**
          * \brief track features in the current Image
          * \param trackedPositions		positions of the tracked 2D features
          * \param trackedFeatureIds		ids (of predictedIds) that have been tracked
          * \param predictedPositions	vector of predicted feature positions
          * \param img					the current image
          */
         virtual void trackFeatures( PointSet2d&						trackedPositions,
                                     std::vector<size_t>&			trackedFeatureIds,
                                     const std::vector<Vector2f>&	predictedPositions,
                                     const std::vector<size_t>&		predictedIds,
                                     const Image&						img ) = 0;

         /* clear all internally cached data if neccessary */
         virtual void clear() = 0;

         Signal<const Image&>	debugImage;

      protected:
         /* triangulate a new point */
         float triangulateSinglePoint( Vector4f&			newPoint,
                                const Vector2f&	point0,
                                const Vector2f&	point1,
                                const Matrix4f&	projMat0,
                                const Matrix4f&	projMat1 ) const;

         bool checkFeatureSAD( const Vector2f& p0, const Vector2f& p1, const Image & i0, const Image & i1 ) const;
   };
}

#endif

