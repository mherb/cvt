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
#include <cvt/vision/slam/stereo/DescriptorDatabase.h>
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
		   struct Params {
				Params():
				   keyframeSelectionRadius( 3.0f ),
				   pyramidOctaves( 4 ),
				   pyramidScaleFactor( 0.6f ),
				   matchingWindow( 50.0f ),
				   matchingMaxDescDistance( 80.0f ),
				   minTrackedFeatures( 70 ),
				   maxKeyframeDistance( 3.0f ),
				   minFeaturesForKeyframe( 10 ),
				   /* TODO: change param to DEPTH range*/
				   minDisparity( 0.1f ),
				   maxDisparity( 20.0f ),
				   maxEpilineDistance( 1.0f ),
				   kltStereoIters( 2 )
				{
				}

				float  keyframeSelectionRadius;
				size_t pyramidOctaves;
				float  pyramidScaleFactor;

				/* radius matching*/
				float  matchingWindow;
				float  matchingMaxDescDistance;

				/* keyframe recreation */
				size_t minTrackedFeatures;
				float  maxKeyframeDistance;
				size_t minFeaturesForKeyframe;

				/* stereo matching params*/
				float minDisparity;
				float maxDisparity;
				float maxEpilineDistance;

				/* klt params */
				size_t kltStereoIters;
		   };

		   StereoSLAM( FeatureDetector* detector,
					   FeatureDescriptorExtractor* descExtractor,
                       const StereoCameraCalibration& calib,
                       const Params& params=Params());


		 /**
		  * @brief newImages
		  * @param imgLeft	undistorted-rectified left frame
		  * @param imgRight undistorted-rectified right frame
		  */
		 void				newImages( const Image& imgLeft,
									   const Image& imgRight );

		 const SlamMap&		map() const { return _map; }

		 void				clear();
		 void				setPose( const Matrix4f& pose );
		 const SE3<float>&	pose() const { return _pose; }

         Signal<const Image&>       newStereoView;
         Signal<const Image&>       trackedFeatureImage;
         Signal<void>               keyframeAdded;
         Signal<const SlamMap&>     mapChanged;
         Signal<const Matrix4f&>    newCameraPose;
         Signal<size_t>             numTrackedPoints;

         EIGEN_MAKE_ALIGNED_OPERATOR_NEW
      private:
		 struct TrackedFeatures {
			PointSet2f			points2d;
			PointSet3f			points3d;

			/* ids of the 3d points within the map */
			std::vector<size_t> mapFeatureIds;

			void reserve( size_t n )
			{
				points2d.reserve( n );
				points3d.reserve( n );
				mapFeatureIds.reserve( n );
			}

			size_t size() const { return points3d.size(); }
		 };

		 typedef DescriptorDatabase::PatchType	PatchType;
		 Params						 _params;
		 FeatureDetector*			 _detector;
		 FeatureDescriptorExtractor* _descExtractorLeft;
		 FeatureDescriptorExtractor* _descExtractorRight;
		 DescriptorDatabase			 _descriptorDatabase;
		 ImagePyramid				 _pyrLeft;
		 ImagePyramid				 _pyrRight;

		 /* float versions for KLT */
		 ImagePyramid				 _pyrLeftf;
		 ImagePyramid				 _pyrRightf;
		 ImagePyramid				 _gradXl;
		 ImagePyramid				 _gradYl;
		 IKernel					 _kernelGx;
		 IKernel					 _kernelGy;


		 StereoCameraCalibration	 _calib;

         /* the current pose of the camera rig */
		 SE3<float>					 _pose;
		 int						 _activeKF;
		 SlamMap					 _map;
		 MapOptimizer				 _bundler;
		 Image						 _lastImage;

		 void extractFeatures( const Image& left, const Image& right );

		 void predictVisibleFeatures( std::vector<Vector2f>& imgPositions,
									  std::vector<size_t>& ids,
									  std::vector<FeatureDescriptor*>& descriptors,
									  std::vector<PatchType*>& patches,
									  const Eigen::Matrix4d& cameraPose );

		 void trackPredictedFeatures( TrackedFeatures& tracked,
									  std::vector<MatchingIndices> &matchedIndices,
									  const std::vector<FeatureDescriptor*>& predictedDescriptors,
									  std::vector<StereoSLAM::PatchType*>& predictedPatches,
									  const std::vector<size_t>& mapIds );

		 void createCorrespondences( PointSet2f& points2d,
									 PointSet3f& points3d,
									 std::vector<size_t>& mapIndices,
									 const std::vector<MatchingIndices>& matchedIndices,
									 const std::vector<size_t>& ids ) const;

		 void estimateCameraPose( std::vector<size_t>& inlierIndices,
								  const PointSet3f & p3d,
								  const PointSet2f & p2d );

		 bool newKeyframeNeeded( size_t numTrackedFeatures ) const;

		 void initNewStereoFeatures( PointSet3f& newPts3d,
									 std::vector<const FeatureDescriptor*>& newDescriptors,
									 std::vector<PatchType*>& newPatches,
									 const std::vector<size_t> &trackingInliers,
									 const std::vector<MatchingIndices>& matchedIndices );

		 void sortOutFreeFeatures( std::vector<const FeatureDescriptor*>& freeFeatures,
								   const FeatureDescriptorExtractor* extractor,
								   const std::vector<size_t>& inliers,
								   const std::vector<MatchingIndices>& matches ) const;

		 void addNewKeyframe( const std::vector<const FeatureDescriptor*>& newDescriptors,
							  std::vector<PatchType *>& newPatches,
							  const PointSet3f& newPoints3d,
							  const PointSet2f& trackedMapPoints,
							  const std::vector<size_t>& trackedMapIds,
							  const std::vector<size_t>& inliers );

         void debugPatchWorkImage( const std::set<size_t>&          indices,
                                   const std::vector<size_t>&       featureIds,
                                   const std::vector<FeatureMatch>& matches );

         void createDebugImageMono( Image & debugImage,
									const PointSet2f & tracked,
                                    const std::vector<Vector2f> & predPos ) const;
   };

}

#endif
