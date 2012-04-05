/*
			CVT - Computer Vision Tools Library

 	 Copyright (c) 2012, Philipp Heise, Sebastian Klose

 	THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 	KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 	IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 	PARTICULAR PURPOSE.
 */
#include <cvt/vision/slam/stereo/ORBTracking.h>

namespace cvt
{
	ORBTracking::ORBTracking( const CameraCalibration & c0, const CameraCalibration & c1 ) :
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

	ORBTracking::~ORBTracking()
	{
	}


	void ORBTracking::trackFeatures( PointSet2d&					trackedPositions,
									std::vector<size_t>&			trackedFeatureIds,
									const std::vector<Vector2f>&	predictedPositions,
									const std::vector<size_t>&		predictedIds,
									const Image&					img )
	{
		// create the ORB
		_orb0.update( img );

		// we want to find the best matching orb feature from current, that lies
		// within a certain distance from the "predicted" position
		std::vector<size_t>::const_iterator currentId = predictedIds.begin();
		std::vector<size_t>::const_iterator tEnd = predictedIds.end();
		std::vector<Vector2f>::const_iterator pred = predictedPositions.begin();

		// keep track of already assigned indices to avoid double associations
		_orb0MatchedIds.clear();
		while( currentId != tEnd ){
			FeatureMatch m;
			const ORBFeature & desc = _descriptors.descriptor( *currentId );
			m.feature0 = &desc;
			int matchedIdx = matchInWindow( m, *pred, _orb0, _orb0MatchedIds ); 
			if( matchedIdx != -1 ){
				_orb0MatchedIds.insert( ( size_t )matchedIdx );				
				trackedPositions.add( Vector2d( m.feature1->pt.x, m.feature1->pt.y ) );
				trackedFeatureIds.push_back( *currentId );
			}
			++currentId;
			++pred;
		}
	}

	int ORBTracking::matchInWindow( FeatureMatch& match, const Vector2f & p, const ORB & orb, const std::set<size_t> & used ) const
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
			return matchedId;
		}
		return -1;
	}

	//size_t ORBTracking::triangulateNewFeatures( SlamMap& map,
	//										    const SE3<double> & pose,
	//										    const Image & firstView,
	//										    const Image & secondView )
	//{
	//	// create the ORB
	//	ORB orb1( secondView, 
	//			 _orbOctaves, 
	//			 _orbScaleFactor,
	//			 _orbCornerThreshold,
	//			 _orbMaxFeatures,
	//			 _orbNonMaxSuppression );

	//	// find stereoMatches by avoiding already found matches
	//	std::vector<FeatureMatch> stereoMatches;

	//	// alreadyMatchedIndices are the indices of orb0 that have been matched before
	//	_stereoMatcher.matchEpipolar( stereoMatches, _orb0, orb1, _orb0MatchedIds );

	//	size_t numNew = 0;
	//	// Create a new keyframe with image 0 as reference image
	//	if( stereoMatches.size() > 0 ){

	//		// add a new keyframe to the map
	//		size_t kId = map.addKeyframe( pose.transformation() );

	//		Keyframe & keyframe = map.keyframeForId( kId );
	//		keyframe.setImage( firstView );

	//		MapMeasurement meas;
	//		// meas.information *= ( 1.0 / _trackingSearchRadius );

	//		Eigen::Matrix4d featureCov = Eigen::Matrix4d::Identity();
	//		MapFeature mapFeat( Eigen::Vector4d::Zero(), featureCov );

	//		// add the currently tracked features to the keyframe!
	//		std::set<size_t>::const_iterator tracked = _orb0MatchedIds.begin();
	//		const std::set<size_t>::const_iterator trackedEnd = _orb0MatchedIds.end();
	//		while( tracked != trackedEnd )
	//		{
	//			size_t pointId = _predictedIds[ *tracked ];
	//			meas.point[ 0 ] = _trackedInFirst[ *tracked ].feature1->pt.x;
	//			meas.point[ 1 ] = _trackedInFirst[ *tracked ].feature1->pt.y;
	//			map.addMeasurement( pointId, kId, meas );
	//			++tracked;
	//		}

	//		Vector2f p0, p1;
	//		Vector4f pNew;

	//		Eigen::Matrix4d poseInv = pose.transformation().inverse();
	//		for( size_t i = 0; i < stereoMatches.size(); i++ ){
	//			const FeatureMatch& match = stereoMatches[ i ];
	//			if( match.feature1 ){
	//				if( checkFeatureSAD( match.feature0->pt, match.feature1->pt, firstView, secondView ) ){
	//					p0 = stereoMatches[ i ].feature0->pt;
	//					p1 = stereoMatches[ i ].feature1->pt;
	//					Vision::correctCorrespondencesSampson( p0, p1, _stereoMatcher.fundamental() );

	//					float reprErr = triangulateSinglePoint( pNew, p0, p1, _camCalib0.projectionMatrix(), _camCalib1.projectionMatrix() );
	//					if( reprErr < _maxTriangReprojError ){
	//						meas.point[ 0 ] = p0.x;
	//						meas.point[ 1 ] = p0.y;

	//						Eigen::Vector4d & point = mapFeat.estimate();
	//						point[ 0 ] = pNew[ 0 ];
	//						point[ 1 ] = pNew[ 1 ];
	//						point[ 2 ] = pNew[ 2 ];
	//						point[ 3 ] = pNew[ 3 ];
	//						point = poseInv * point;

	//						size_t newPointId = map.addFeatureToKeyframe( mapFeat, meas, kId );
	//						_descriptors.addDescriptor( *( ORBFeature* )stereoMatches[ i ].feature0, newPointId );
	//						numNew++;
	//					}
	//				}
	//			}
	//		}
	//	}
	//	return numNew;
	//}

	void ORBTracking::clear()
	{
		_descriptors.clear();
	}
}

