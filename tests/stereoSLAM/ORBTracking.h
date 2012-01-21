#ifndef ORB_TRACKING_H
#define ORB_TRACKING_H

#include "FeatureTracking.h"
#include <cvt/vision/slam/MapFeature.h>
#include "DescriptorDatabase.h"
#include <cvt/vision/ORB.h>
#include <cvt/vision/Vision.h>

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

	inline ORBTracking::ORBTracking( const CameraCalibration & c0, const CameraCalibration & c1 ) :
		_camCalib0( c0 ),
		_camCalib1( c1 ),
		_maxDescDistance( 80 ),
		_windowRadius( 25 ),
		_matcherMaxLineDistance( 5.0f ),
		_maxTriangReprojError( 5.0f ),
		_stereoMatcher( _matcherMaxLineDistance, _maxDescDistance, c0, c1 ),
		_orbOctaves( 4 ), 
		_orbScaleFactor( 0.5f ),
		_orbCornerThreshold( 20 ),
		_orbMaxFeatures( 800 ),
		_orbNonMaxSuppression( true ),
		_orb0( _orbOctaves, _orbScaleFactor, _orbCornerThreshold, _orbMaxFeatures, _orbNonMaxSuppression )
	{
	}

	inline ORBTracking::~ORBTracking()
	{
	}

    inline void ORBTracking::trackFeatures( PointSet3d&			p3d, 
										    PointSet2d&			p2d,
											const SlamMap&		map,
											const SE3<double>&	pose,
                                            const Image&		img )
    {
		// create the ORB
		_orb0.update( img );

		// predict visible features with map and current pose
		_predictedIds.clear();
		_predictedPositions.clear();
		map.selectVisibleFeatures( _predictedIds, _predictedPositions, pose.transformation(), _camCalib0 );

        // we want to find the best matching orb feature from current, that lies
        // within a certain distance from the "predicted" position
        std::vector<size_t>::const_iterator currentId = _predictedIds.begin();
        std::vector<size_t>::const_iterator tEnd = _predictedIds.end();
        std::vector<Vector2f>::const_iterator pred = _predictedPositions.begin();

		// keep track of already assigned indices to avoid double associations
		std::set<size_t> used;
		_trackedInFirst.clear();
        while( currentId != tEnd ){
			FeatureMatch m;
			const ORBFeature & desc = _descriptors.descriptor( *currentId );
			m.feature0 = &desc;
            matchInWindow( m, *pred, _orb0, used );
			_trackedInFirst.push_back( m );
			++currentId;
			++pred;
        }

		_orb0MatchedIds.clear();
		correspondencesFromMatchedFeatures( p3d, p2d, _orb0MatchedIds, map, img );
    }

    inline void ORBTracking::matchInWindow( FeatureMatch& match, const Vector2f & p, const ORB & orb, std::set<size_t> & used ) const
    {
		const ORBFeature * f = (ORBFeature*)match.feature0;
        match.distance = _maxDescDistance;
        size_t currDist;
		const std::set<size_t>::const_iterator usedEnd = used.end();
		size_t matchedId = 0;
        for( size_t i = 0; i < orb.size(); i++ ){
			if( used.find( i ) == usedEnd ){
				if( ( p - orb[ i ].pt ).length() < _windowRadius ){
					// try to match
					currDist = f->distance( orb[ i ] );
					if( currDist < match.distance ){
						match.feature1 = &orb[ i ];
						match.distance = currDist;
						matchedId = i;
					}
				}
			}
        }

		// to ensure unique matches
		if( match.distance < _maxDescDistance ){
			used.insert( matchedId );
		}
    }

	inline size_t ORBTracking::triangulateNewFeatures( SlamMap& map,
													   const SE3<double> & pose,
													   const Image & firstView,
												       const Image & secondView )
	{
		// create the ORB
		ORB orb1( secondView, 
				 _orbOctaves, 
				 _orbScaleFactor,
				 _orbCornerThreshold,
				 _orbMaxFeatures,
				 _orbNonMaxSuppression );

		// find stereoMatches by avoiding already found matches
		std::vector<FeatureMatch> stereoMatches;

		// alreadyMatchedIndices are the indices of orb0 that have been matched before
		_stereoMatcher.matchEpipolar( stereoMatches, _orb0, orb1, _orb0MatchedIds );

		size_t numNew = 0;
		// Create a new keyframe with image 0 as reference image
		if( stereoMatches.size() > 0 ){

			// add a new keyframe to the map
			size_t kId = map.addKeyframe( pose.transformation() );

			Keyframe & keyframe = map.keyframeForId( kId );
			keyframe.setImage( firstView );

			MapMeasurement meas;
			// meas.information *= ( 1.0 / _trackingSearchRadius );

			Eigen::Matrix4d featureCov = Eigen::Matrix4d::Identity();
			MapFeature mapFeat( Eigen::Vector4d::Zero(), featureCov );

			// add the currently tracked features to the keyframe!
			std::set<size_t>::const_iterator tracked = _orb0MatchedIds.begin();
			const std::set<size_t>::const_iterator trackedEnd = _orb0MatchedIds.end();
			while( tracked != trackedEnd )
			{
				size_t pointId = _predictedIds[ *tracked ];
				meas.point[ 0 ] = _trackedInFirst[ *tracked ].feature1->pt.x;
				meas.point[ 1 ] = _trackedInFirst[ *tracked ].feature1->pt.y;
				map.addMeasurement( pointId, kId, meas );
				++tracked;
			}

			Vector2f p0, p1;
			Vector4f pNew;

			Eigen::Matrix4d poseInv = pose.transformation().inverse();
			for( size_t i = 0; i < stereoMatches.size(); i++ ){
				const FeatureMatch& match = stereoMatches[ i ];
				if( match.feature1 ){
					if( checkFeatureSAD( match.feature0->pt, match.feature1->pt, firstView, secondView ) ){
						p0 = stereoMatches[ i ].feature0->pt;
						p1 = stereoMatches[ i ].feature1->pt;
						Vision::correctCorrespondencesSampson( p0, p1, _stereoMatcher.fundamental() );

						float reprErr = triangulateSinglePoint( pNew, p0, p1, _camCalib0.projectionMatrix(), _camCalib1.projectionMatrix() );
						if( reprErr < _maxTriangReprojError ){
							meas.point[ 0 ] = p0.x;
							meas.point[ 1 ] = p0.y;

							Eigen::Vector4d & point = mapFeat.estimate();
							point[ 0 ] = pNew[ 0 ];
							point[ 1 ] = pNew[ 1 ];
							point[ 2 ] = pNew[ 2 ];
							point[ 3 ] = pNew[ 3 ];
							point = poseInv * point;

							size_t newPointId = map.addFeatureToKeyframe( mapFeat, meas, kId );
							_descriptors.addDescriptor( *( ORBFeature* )stereoMatches[ i ].feature0, newPointId );
							numNew++;
						}
					}
				}
			}
		}
		return numNew;
	}

	inline void ORBTracking::correspondencesFromMatchedFeatures( PointSet3d& p3d, 
															     PointSet2d& p2d,
															     std::set<size_t>& matchedIndices,
															     const SlamMap & map, 
															     const Image& img )
	{
		Vector3d p3;
		Vector2d p2;

		for( size_t i = 0; i < _trackedInFirst.size(); i++ ){
			if( _trackedInFirst[ i ].feature1 ){
				// got a match so add it to the point sets
				const MapFeature & mapFeat = map.featureForId( _predictedIds[ i ] );
				size_t keyframeId = *( mapFeat.pointTrackBegin() );
				const FeatureMatch& match = _trackedInFirst[ i ];
				if( checkFeatureSAD( match.feature0->pt, match.feature1->pt, 
									 map.keyframeForId( keyframeId ).image(), img ) ) {
					p3.x = mapFeat.estimate().x(); 
					p3.y = mapFeat.estimate().y(); 
					p3.z = mapFeat.estimate().z();
					p3d.add( p3 );
					p2.x = match.feature1->pt.x;
					p2.y = match.feature1->pt.y;
					p2d.add( p2 );
					matchedIndices.insert( i );
				}
			}
		}
	}

	inline void ORBTracking::clear()
	{
		_descriptors.clear();
	}
}
#endif
