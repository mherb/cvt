#include <cvt/vision/slam/stereo/ORBTrackingSequential.h>

namespace cvt
{
	ORBTrackingSequential::ORBTrackingSequential( const CameraCalibration & c0, const CameraCalibration & c1 ) :
		_camCalib0( c0 ),
		_camCalib1( c1 ),
		_maxDescDistance( 40 ),
		_windowRadius( 20 ),
		_matcherMaxLineDistance( 7.0f ),
		_maxTriangReprojError( 3.0f ),
		_stereoMatcher( _matcherMaxLineDistance, _maxDescDistance, c0, c1 ),
		_orbOctaves( 3 ), 
		_orbScaleFactor( 0.5f ),
		_orbCornerThreshold( 30 ),
		_orbMaxFeatures( 1000 ),
		_orbNonMaxSuppression( true ),
		_orb0( _orbOctaves, _orbScaleFactor, _orbCornerThreshold, _orbMaxFeatures, _orbNonMaxSuppression )
	{
	}

	ORBTrackingSequential::~ORBTrackingSequential()
	{
	}

	void ORBTrackingSequential::trackFeatures( PointSet3d&			p3d, 
											  PointSet2d&			p2d,
											  const SlamMap&		map,
											  const SE3<double>&	pose,
											  const Image&			img )
	{
		// create the ORB
		_orb0.update( img );
		img.convert( _debug, IFormat::RGBA_UINT8 );

		drawOrbsInImage( _debug, Color::BLUE, _orb0 );
		drawOrbsInImage( _debug, Color::RED, _lastTrackedFeatures );
		trackSequential( p3d, p2d, map, img );
		drawPointsInImage( _debug, Color::GREEN, p2d );

		size_t offset = p2d.size();
		trackNonSequential( p3d, p2d, map, pose, img );
		drawPointsInImage( _debug, Color::GREEN, p2d, offset );
		debugImage.notify( _debug );
	}

	void ORBTrackingSequential::trackSequential( PointSet3d&		p3d, 
												 PointSet2d&		p2d,
												 const SlamMap&		map,
												 const Image&		img )
	{
		// keep track of already assigned indices to avoid double associations
		Vector3d p3;
		Vector2d p2;

		_orb0MatchedIds.clear();
		size_t savePos = 0;
		for( size_t i = 0; i < _lastTrackedFeatures.size(); i++ ){
			FeatureMatch m;
			m.feature0 = &_lastTrackedFeatures[ i ];
			int orbid = matchInWindow( m, _lastTrackedFeatures[ i ].pt, _orb0, _orb0MatchedIds );
			if( m.feature1 ){
				// got a match so add it to the point sets
				size_t currId = _lastTrackedIds[ i ];
				const MapFeature & mapFeat = map.featureForId( currId );
				size_t keyframeId = *( mapFeat.pointTrackBegin() );
				if( checkFeatureSAD( m.feature0->pt, m.feature1->pt, 
									map.keyframeForId( keyframeId ).image(), img ) ) {
					p3.x = mapFeat.estimate().x(); 
					p3.y = mapFeat.estimate().y(); 
					p3.z = mapFeat.estimate().z();
					p3d.add( p3 );
					p2.x = m.feature1->pt.x;
					p2.y = m.feature1->pt.y;
					p2d.add( p2 );
					_orb0MatchedIds.insert( orbid );
					_lastTrackedIds[ savePos ] = currId;
					_lastTrackedFeatures[ savePos ] = *( ORBFeature* )m.feature1;
					savePos++;					
				}
			}
		}

		_lastTrackedFeatures.erase( _lastTrackedFeatures.begin() + savePos, _lastTrackedFeatures.end() );
		_lastTrackedIds.erase( _lastTrackedIds.begin() + savePos, _lastTrackedIds.end() );
	}


	void ORBTrackingSequential::trackNonSequential( PointSet3d&			p3d, 
												   PointSet2d&			p2d,
												   const SlamMap&		map,
												   const SE3<double>&	pose,
												   const Image&			img )
	{
		// predict visible features with map and current pose
		_predictedIds.clear();
		_predictedPositions.clear();
		map.selectVisibleFeatures( _predictedIds, _predictedPositions, pose.transformation(), _camCalib0 );

		drawPointsInImage( _debug, Color::YELLOW, _predictedPositions );

		// remove the already tracked ids:
		bool hit;

		size_t savePos = 0;
		for( size_t k = 0; k < _predictedIds.size(); k++ ){
			hit = false;
			for( size_t i = 0; i < _lastTrackedIds.size(); i++ ){
				if( _lastTrackedIds[ i ] == _predictedIds[ k ] ){
					hit = true;
					break;
				}
			}
			if( !hit ){
				if( savePos != k ){
					_predictedIds[ savePos ] = _predictedIds[ k ];
					_predictedPositions[ savePos ] = _predictedPositions[ k ];
					savePos++;
				}
			}
		}
		_predictedIds.erase( _predictedIds.begin() + savePos, _predictedIds.end() );
		_predictedPositions.erase( _predictedPositions.begin() + savePos, _predictedPositions.end() );

		// we want to find the best matching orb feature from current, that lies
		// within a certain distance from the "predicted" position
		std::vector<size_t>::const_iterator currentId = _predictedIds.begin();
		std::vector<size_t>::const_iterator tEnd = _predictedIds.end();
		std::vector<Vector2f>::const_iterator pred = _predictedPositions.begin();

		// keep track of already assigned indices to avoid double associations
		Vector3d p3;
		Vector2d p2;
		while( currentId != tEnd ){
			FeatureMatch m;
			const ORBFeature & desc = _descriptors.descriptor( *currentId );
			m.feature0 = &desc;
			int orbid = matchInWindow( m, *pred, _orb0, _orb0MatchedIds );
			if( m.feature1 ){
				// got a match so add it to the point sets
				const MapFeature & mapFeat = map.featureForId( *currentId );
				size_t keyframeId = *( mapFeat.pointTrackBegin() );
				if( checkFeatureSAD( m.feature0->pt, m.feature1->pt, 
									map.keyframeForId( keyframeId ).image(), img ) ) {
					p3.x = mapFeat.estimate().x(); 
					p3.y = mapFeat.estimate().y(); 
					p3.z = mapFeat.estimate().z();
					p3d.add( p3 );
					p2.x = m.feature1->pt.x;
					p2.y = m.feature1->pt.y;
					p2d.add( p2 );
					_orb0MatchedIds.insert( orbid );
					_lastTrackedIds.push_back( *currentId );
					_lastTrackedFeatures.push_back( *( ORBFeature* )m.feature1 );
				}
			}
			++currentId;
			++pred;
		}
	}

	int ORBTrackingSequential::matchInWindow( FeatureMatch& match, const Vector2f & p, const ORB & orb, std::set<size_t> & used ) const
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
			return ( int )matchedId;
		}

		return -1;
	}

	size_t ORBTrackingSequential::triangulateNewFeatures( SlamMap& map,
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

		//size_t savepos = 0;
		//for( size_t i = 0; i < stereoMatches.size(); i++ ){
		//	if( rangeCheck( stereoMatches[ i ].feature1->pt, 60.0f ) ){
		//		if( i != savepos ){
		//			stereoMatches[ savepos ] = stereoMatches[ i ];
		//			savepos++;
		//		}
		//	}
		//}
		//stereoMatches.erase( stereoMatches.begin() + savepos, stereoMatches.end() );

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
			for( size_t i = 0; i < _lastTrackedIds.size(); i++ ){				
				size_t pointId = _lastTrackedIds[ i ];
				meas.point[ 0 ] = _lastTrackedFeatures[ i ].pt.x;
				meas.point[ 1 ] = _lastTrackedFeatures[ i ].pt.y;
				map.addMeasurement( pointId, kId, meas );
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
							_lastTrackedIds.push_back( newPointId );
							_lastTrackedFeatures.push_back( *( ORBFeature* )stereoMatches[ i ].feature0 );
							numNew++;
						}
					}
				}
			}
		}
		std::cout << "New Features Triangulated: " << numNew << std::endl;
		return numNew;
	}

	void ORBTrackingSequential::clear()
	{
		_descriptors.clear();
		_lastTrackedIds.clear();
		_lastTrackedFeatures.clear();
	}

	bool ORBTrackingSequential::rangeCheck( const Vector2f& p, float minDist ) const
	{
		float thresh = Math::sqr( minDist );
		for( size_t i = 0; i < _predictedPositions.size(); i++ ){
			if( ( p - _predictedPositions[ i ] ).lengthSqr() < thresh )
				return false;
		}
		for( size_t i = 0; i < _lastTrackedFeatures.size(); i++ ){
			if( ( p - _lastTrackedFeatures[ i ].pt ).lengthSqr() < thresh )
				return false;
		}
		return true;
	}


}
