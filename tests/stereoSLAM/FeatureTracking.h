#ifndef CVT_FEATURE_TRACKING_H
#define CVT_FEATURE_TRACKING_H

#include <vector>
#include <cvt/vision/FeatureMatch.h>
#include <cvt/geom/PointSet.h>

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

			virtual bool checkFeature( const FeatureMatch& match, 
									   const Image& i0, 
							   		   const Image& i1 ) const = 0;

			virtual void clear() = 0;


			virtual size_t triangulateNewFeatures( SlamMap& map, 
												   const SE3<double>& pose, 
												   const Image& first, 
												   const Image& second ) = 0;
	};
}

#endif
