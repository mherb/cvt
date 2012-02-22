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
#include <cvt/vision/Vision.h>
#include <cvt/vision/FeatureMatch.h>
#include <cvt/vision/slam/SlamMap.h>
#include <cvt/geom/PointSet.h>
#include <cvt/util/Signal.h>

namespace cvt
{
	class FeatureTracking
	{
		public:
			virtual ~FeatureTracking() {}

			virtual void trackFeatures( PointSet3d&			p3d, 
									    PointSet2d&			p2d,
									    const SlamMap&		map,
									    const SE3<double>&	pose,
									    const Image&		img ) = 0;

			virtual void clear() = 0;


			virtual size_t triangulateNewFeatures( SlamMap& map, 
												  const SE3<double>& pose, 
												  const Image& first, 
												  const Image& second ) = 0;

			Signal<const Image&>	debugImage;

		protected:
			/* triangulate a new point */	
			virtual float triangulateSinglePoint( Vector4f&			newPoint, 
												 const Vector2f&	point0,
												 const Vector2f&	point1,
												 const Matrix4f&	projMat0,
												 const Matrix4f&	projMat1 ) const;

			bool checkFeatureSAD( const Vector2f& p0, const Vector2f& p1, const Image & i0, const Image & i1 ) const;
	};
}

#endif

