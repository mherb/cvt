/*
         CVT - Computer Vision Tools Library

    Copyright (c) 2012, Philipp Heise, Sebastian Klose

   THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
   KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
   IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
   PARTICULAR PURPOSE.
 */
#ifndef CVT_STEREO_SLAM_H
#define CVT_STEREO_SLAM_H

#include <cvt/gfx/Image.h>
#include <cvt/math/SE3.h>
#include <cvt/util/Signal.h>
#include <cvt/vision/slam/SlamMap.h>
#include <cvt/vision/slam/Keyframe.h>
#include <cvt/vision/features/FeatureMatch.h>
#include <cvt/vision/CameraCalibration.h>
#include <cvt/vision/StereoCameraCalibration.h>
#include <cvt/vision/slam/stereo/FeatureTracking.h>
#include <cvt/vision/slam/stereo/MapOptimizer.h>

#include <set>

namespace cvt
{
   // Managing class for stereo SLAM
   class StereoSLAM
   {
      public:
		 StereoSLAM( FeatureTracking* ft, const StereoCameraCalibration& calib );

		 /**
		  * @brief newImages
		  * @param imgLeft	undistorted-rectified left frame
		  * @param imgRight undistorted-rectified right frame
		  */
		 void newImages( const Image& imgLeft, const Image& imgRight );

         const SlamMap & map() const { return _map; }
         void clear();

         void setPose( const Matrix4d& pose );
         const SE3<double>& pose() const { return _pose; }

         Signal<const Image&>       newStereoView;
         Signal<const Image&>       trackedFeatureImage;
         Signal<void>               keyframeAdded;
         Signal<const SlamMap&>     mapChanged;
         Signal<const Matrix4f&>    newCameraPose;
         Signal<size_t>             numTrackedPoints;

      private:
		 FeatureTracking*			_featureTracking;
		 StereoCameraCalibration	_calib;

         // minimum needed features before new keyframe is added
         size_t             _minTrackedFeatures;

         /* the current pose of the camera rig */
         SE3<double>        _pose;

         /* the active Keyframe Id (closest to _pose) */
         int                _activeKF;
         double             _minKeyframeDistance;
         double             _maxKeyframeDistance;

         SlamMap            _map;

         MapOptimizer       _bundler;
         Image              _lastImage;

         void estimateCameraPose( std::vector<size_t>& inlierIndices, const PointSet3d & p3d, const PointSet2d & p2d );

         void debugPatchWorkImage( const std::set<size_t>&          indices,
                                   const std::vector<size_t>&       featureIds,
                                   const std::vector<FeatureMatch>& matches );

         bool newKeyframeNeeded( size_t numTrackedFeatures ) const;

         void fillPointsetFromIds( PointSet3d& pset,
                                   const std::vector<size_t>& ids ) const;

		 /*
         void addNewKeyframe( const std::vector<DepthInitializer::DepthInitResult> & triangulated,
                              const PointSet2d& p2d,
                              const std::vector<size_t>& trackedIds,
                              const std::vector<size_t>& inliers );
		*/

         void createDebugImageMono( Image & debugImage,
                                    const PointSet2d & tracked,
                                    const std::vector<Vector2f> & predPos ) const;

		 //void createDebugImageStereo( Image & debugImage,
		 //                             const std::vector<DepthInitializer::DepthInitResult>& triang ) const;
   };

}

#endif
