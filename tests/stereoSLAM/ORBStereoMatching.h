#ifndef ORB_STEREO_MATCHING_H
#define ORB_STEREO_MATCHING_H

#include <cvt/vision/ORB.h>
#include <cvt/geom/Line2D.h>

namespace cvt
{
	class ORBStereoMatching
	{
		public:
			ORBStereoMatching( float maxLineDistance,
							   float maxDescriptorDistance, 
							   const CameraCalibration & c0, 
							   const CameraCalibration & c1 );

			// assuming orb features are undistorted!
			void matchEpipolar( std::vector<FeatureMatch>& matches, const ORB& orb0, const ORB& orb1 ) const;

		private:
			Matrix3f	_fundamental;
			float		_maxLineDist;
			float		_maxDescDist;
	};

	inline ORBStereoMatching::ORBStereoMatching( float maxLineDistance,
												 float maxDescriptorDistance,	
											     const CameraCalibration& c0,
												 const CameraCalibration& c1 ) :
		_maxLineDist( maxLineDistance ),
		_maxDescDist( maxDescriptorDistance )
	{
		// calc the essential matrix from the calibration data
		Vision::composeFundamental( _fundamental, 
								    c0.intrinsics(),
								    c0.extrinsics(),
								    c1.intrinsics(),
								   	c1.extrinsics() );
	}

	inline void ORBStereoMatching::matchEpipolar( std::vector<FeatureMatch>& matches, const ORB& orb0, const ORB& orb1 ) const
	{
		// with a row lookup table, we could probably speed up the search
		Vector3f line, point;
		FeatureMatch match;
		point.z = 1.0f;
		size_t distance;

		for( size_t i = 0; i < orb0.size(); i++ ){
			const ORBFeature & f = orb0[ i ];
			point.x = f.pt.x;
			point.y = f.pt.y;
			line = _fundamental * point;
			Line2Df l( line );

			match.distance = _maxDescDist;
			match.feature0 = &orb0[ i ];

			for( size_t k = 0; k < orb1.size(); k++ ){
				float lDist = l.distance( orb1[ k ].pt );
				if( Math::abs( l.distance( orb1[ k ].pt ) ) < _maxLineDist ){
					// check descriptor distance
					distance = f.distance( orb1[ k ] );
					std::cout << "Line dist: " << lDist << " descDist: " << distance << std::endl;
					if( distance < match.distance ){
						match.feature1 = &orb1[ k ];
						match.distance = distance;
					}
				}	
			}

			if( match.distance < _maxDescDist ){
				matches.push_back( match );
			}
		}
	}
}

#endif
