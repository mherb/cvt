/*
			CVT - Computer Vision Tools Library

 	 Copyright (c) 2012, Philipp Heise, Sebastian Klose

 	THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 	KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 	IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 	PARTICULAR PURPOSE.
 */
#ifndef CVT_KLT_FEATURE_TRACKING_H
#define CVT_KLT_FEATURE_TRACKING_H

#include <cvt/vision/slam/stereo/FeatureTracking.h>
#include <cvt/vision/slam/stereo/ORBStereoMatching.h>

#include <cvt/vision/KLTTracker.h>

namespace cvt
{
	class KLTTracking : public FeatureTracking
	{
		public:
			KLTTracking( const CameraCalibration& c0, const CameraCalibration& c1 );


			void trackFeatures( PointSet3d&			p3d, 
								PointSet2d&			p2d,
								const SlamMap&		map,
								const SE3<double>&	pose,
								const Image&		img );

			bool checkFeature( const FeatureMatch& match, 
							   const Image& i0, 
							   const Image& i1 ) const;

			void clear();

			size_t triangulateNewFeatures( SlamMap& map, 
										   const SE3<double>& pose, 
										   const Image& first, 
										   const Image& second );

		private:
			typedef Translation2D<float> PoseType;

			const CameraCalibration&	_camCalib0;
			const CameraCalibration&	_camCalib1;
			ORBStereoMatching			_stereoMatcher;
			float						_maxTriangReprojError;

			std::vector<size_t>			_predictedIds;
			std::vector<Vector2f>		_predictedPositions;
			std::vector<size_t>			_trackedIndices;
			
			KLTTracker<PoseType>		_klt;
			std::vector<Vector2f>		_trackedPositions;

			typedef KLTPatch<16, PoseType> PatchType;
			std::vector<PatchType*>		_patchForId;
	};
}
#endif

