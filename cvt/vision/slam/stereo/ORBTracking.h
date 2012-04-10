/*
         CVT - Computer Vision Tools Library

    Copyright (c) 2012, Philipp Heise, Sebastian Klose

   THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
   KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
   IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
   PARTICULAR PURPOSE.
 */
#ifndef ORB_TRACKING_H
#define ORB_TRACKING_H

#include <cvt/vision/Vision.h>
#include <cvt/vision/ORB.h>
#include <cvt/vision/slam/MapFeature.h>
#include <cvt/vision/slam/stereo/DescriptorDatabase.h>
#include <cvt/vision/slam/stereo/FeatureTracking.h>
#include <cvt/vision/slam/stereo/ORBStereoMatching.h>

namespace cvt
{

   /**
    *	Feature Tracking using ORB Matching
    */
   class ORBTracking : public FeatureTracking
   {
      public:
         ORBTracking();
         ~ORBTracking();

         void trackFeatures( PointSet2d&                    trackedPositions,
                             std::vector<size_t>&           trackedFeatureIds,
                             const std::vector<Vector2f>&   predictedPositions,
                             const std::vector<size_t>&     predictedIds,
                             const Image&                   img );

         void addFeatureToDatabase( const Vector2f & f, size_t id );

         void clear();

      private:
         DescriptorDatabase<ORBFeature> _descriptors;
         size_t                         _maxDescDistance;
         float                          _windowRadius;

         /* orb parameters */
         size_t                         _orbOctaves;
         float							_orbScaleFactor;
         uint8_t						_orbCornerThreshold;
         size_t                         _orbMaxFeatures;
         bool							_orbNonMaxSuppression;
         ORB							_orb0;
         std::set<size_t>               _orb0MatchedIds;

         int matchInWindow( FeatureMatch& match, const Vector2f & p, const ORB & orb, const std::set<size_t> & used ) const;

   };
}

#endif

