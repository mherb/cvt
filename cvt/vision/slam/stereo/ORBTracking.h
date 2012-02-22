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
			ORBTracking( const CameraCalibration& c0, const CameraCalibration & c1 );
			~ORBTracking();

			void trackFeatures( PointSet3d&			p3d, 
								PointSet2d&			p2d,
								const SlamMap&		map,
								const SE3<double>&	pose,
                                const Image&		img );

			size_t triangulateNewFeatures( SlamMap& map, 
										   const SE3<double>& pose, 
										   const Image& first, 
										   const Image& second );

			void clear();

		private:
			const CameraCalibration&		_camCalib0;
			const CameraCalibration&		_camCalib1;
			DescriptorDatabase<ORBFeature>	_descriptors;
            size_t							_maxDescDistance;
            float							_windowRadius;

			/* descriptor matching parameters */
			float							_matcherMaxLineDistance;	
			float							_maxTriangReprojError;
			ORBStereoMatching				_stereoMatcher;

			/* orb parameters */
			size_t							_orbOctaves;
			float							_orbScaleFactor;
			uint8_t							_orbCornerThreshold;
			size_t							_orbMaxFeatures;
			bool							_orbNonMaxSuppression;
			ORB								_orb0;
			std::set<size_t>				_orb0MatchedIds;
			std::vector<FeatureMatch>		_trackedInFirst;

			std::vector<size_t>				_predictedIds;
			std::vector<Vector2f>			_predictedPositions;

			void matchInWindow( FeatureMatch& match, const Vector2f & p, const ORB & orb, std::set<size_t> & used ) const;

			void correspondencesFromMatchedFeatures( PointSet3d& p3d, PointSet2d& p2d,
													 std::set<size_t>& matchedIndices,
													 const SlamMap & map, 
													 const Image& img );

	};
}

#endif

