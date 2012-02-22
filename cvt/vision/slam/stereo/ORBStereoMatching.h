/*
			CVT - Computer Vision Tools Library

 	 Copyright (c) 2012, Philipp Heise, Sebastian Klose

 	THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 	KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 	IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 	PARTICULAR PURPOSE.
 */
#ifndef ORB_STEREO_MATCHING_H
#define ORB_STEREO_MATCHING_H

#include <cvt/vision/ORB.h>
#include <cvt/geom/Line2D.h>

#include <set>

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
			void matchEpipolar( std::vector<FeatureMatch>& matches, const ORB& orb0, const ORB& orb1, const std::set<size_t> & usedIndices ) const;

			size_t matchEpipolar( FeatureMatch& m, const ORB& orb1, const std::set<size_t>& usedIn1 ) const;

			const Matrix3f & fundamental() const { return _fundamental; }

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
			
	inline size_t ORBStereoMatching::matchEpipolar( FeatureMatch& m, const ORB& orb1, const std::set<size_t>& usedIn1 ) const
	{ 
		Vector3f point;
		point.z = 1.0f;
		
		m.distance = _maxDescDist;
		size_t distance;
		

		point.x = m.feature0->pt.x;
		point.y = m.feature0->pt.y;
		Line2Df l( _fundamental * point );

		size_t currAssigned = 0;
		const std::set<size_t>::const_iterator usedEnd = usedIn1.end();
		const ORBFeature& f = ( const ORBFeature& )*m.feature0;
		for( size_t k = 0; k < orb1.size(); k++ ){
			if( usedIn1.find( k ) == usedEnd ) {
				float lDist = l.distance( orb1[ k ].pt );
				if( Math::abs( lDist ) < _maxLineDist ){
					// check descriptor distance
					distance = f.distance( orb1[ k ] );
					if( distance < m.distance ){
						m.feature1 = &orb1[ k ];
						m.distance = distance;
						currAssigned = k;
					}
				}	
			}
		}
		return currAssigned;
	}

	inline void ORBStereoMatching::matchEpipolar( std::vector<FeatureMatch>& matches, const ORB& orb0, const ORB& orb1, const std::set<size_t> & used ) const
	{
		// with a row lookup table, we could probably speed up the search
		Vector3f line, point;
		point.z = 1.0f;
		FeatureMatch match;
		size_t distance;

		std::set<size_t>::const_iterator usedEnd = used.end();

		// ids from orb1 that have been assigned to a feature of orb1
		std::set<size_t> assigned;

		for( size_t i = 0; i < orb0.size(); i++ ){
			if( used.find( i ) != usedEnd )
				continue;

			const ORBFeature & f = orb0[ i ];
			point.x = f.pt.x;
			point.y = f.pt.y;
			line = _fundamental * point;
			Line2Df l( line );

			match.distance = _maxDescDist;
			match.feature0 = &orb0[ i ];
			
			size_t currAssigned = 0;
			const std::set<size_t>::const_iterator assignedEnd = assigned.end();
			for( size_t k = 0; k < orb1.size(); k++ ){
				if( assigned.find( k ) == assignedEnd ) {
					float lDist = l.distance( orb1[ k ].pt );
					if( Math::abs( lDist ) < _maxLineDist ){
						// check descriptor distance
						distance = f.distance( orb1[ k ] );
						if( distance < match.distance ){
							match.feature1 = &orb1[ k ];
							match.distance = distance;
							currAssigned = k;
						}
					}	
				}
			}

			if( match.distance < _maxDescDist ){
				matches.push_back( match );
				assigned.insert( currAssigned );
			}
		}
	}
}

#endif
