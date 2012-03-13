/*
			CVT - Computer Vision Tools Library

 	 Copyright (c) 2012, Philipp Heise, Sebastian Klose

 	THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 	KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 	IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 	PARTICULAR PURPOSE.
 */
#include <cvt/vision/slam/stereo/ORBTrackingSequential.h>
#include <cvt/vision/FeatureFilter.h>

namespace cvt
{
	ORBTrackingSequential::ORBTrackingSequential( const CameraCalibration & c0, const CameraCalibration & c1 ) :
		_camCalib0( c0 ),
		_camCalib1( c1 ),
		_maxDescDistance( 80 ),
		_windowRadius( 60 ),
		_matcherMaxLineDistance( 2.0f ),
		_maxTriangReprojError( 2.0f ),
		_stereoMatcher( _matcherMaxLineDistance, _maxDescDistance, c0, c1 ),
		_orbOctaves( 3 ), 
		_orbScaleFactor( 0.5f ),
		_orbCornerThreshold( 40 ),
		_orbMaxFeatures( 0 /* all */ ),
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
		_orb0MatchedIds.clear();
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
												    const Image&		img )
	{
		// predict visible features with map and current pose
		_predictedIds.clear();
		_predictedPositions.clear();
		map.selectVisibleFeatures( _predictedIds, _predictedPositions, pose.transformation(), _camCalib0, 1.5 );

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
		float distSqr = Math::sqr( _windowRadius );
		for( size_t i = 0; i < orb.size(); i++ ){
			if( used.find( i ) == usedEnd ){
				if( ( p - orb[ i ].pt ).lengthSqr() < distSqr ){
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
				 false );

		FeatureFilter filter( 20, firstView.width(), firstView.height() );

		const std::set<size_t>::const_iterator endIter = _orb0MatchedIds.end();
		for( size_t i = 0; i < _orb0.size(); i++ ){
			if( _orb0MatchedIds.find( i ) == endIter ){
				if( rangeCheck( _orb0[ i ].pt, 20.0f ) ){
					filter.addFeature( &_orb0[ i ] );
				}
			}
		}

		std::vector<const Feature2Df*> filteredFeatures0;
		filter.gridFilteredFeatures( filteredFeatures0, 1000 );

		// find stereoMatches by avoiding already found matches
		std::vector<FeatureMatch> stereoMatches;

		FeatureMatch match;
		std::set<size_t> orb1Assigned;
		Vector2f p0, p1;
		Vector4f pNew;

		std::vector<const ORBFeature*> triangulatedFeat;
		std::vector<Vector4f> triangulatedPoint;
		std::vector<Vector2f> triangulatedMeas;

		for( size_t i = 0; i < filteredFeatures0.size(); i++ ){
			match.feature0 = filteredFeatures0[ i ];

			size_t id = _stereoMatcher.matchEpipolar( match, orb1, orb1Assigned );
			if( match.distance < _maxDescDistance && 
				checkFeatureSAD( match.feature0->pt, match.feature1->pt, firstView, secondView ) ){
				p0 = match.feature0->pt;
				p1 = match.feature1->pt;
				Vision::correctCorrespondencesSampson( p0, p1, _stereoMatcher.fundamental() );

				float reprErr = triangulateSinglePoint( pNew, p0, p1, _camCalib0.projectionMatrix(), _camCalib1.projectionMatrix() );
				if( reprErr < _maxTriangReprojError ){
					triangulatedPoint.push_back( pNew );
					triangulatedMeas.push_back( p0 );
					triangulatedFeat.push_back( (const ORBFeature*)match.feature0 );
					orb1Assigned.insert( id );
				} 
			} 
		}

		if( triangulatedPoint.size() > 15 ){
			// Create a new keyframe with image 0 as reference image
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

			Eigen::Matrix4d poseInv = pose.transformation().inverse();
			for( size_t i = 0; i < triangulatedPoint.size(); i++ ){
				meas.point[ 0 ] = triangulatedMeas[ i ].x;
				meas.point[ 1 ] = triangulatedMeas[ i ].y;

				Eigen::Vector4d & point = mapFeat.estimate();
				point[ 0 ] = triangulatedPoint[ i ][ 0 ];
				point[ 1 ] = triangulatedPoint[ i ][ 1 ];
				point[ 2 ] = triangulatedPoint[ i ][ 2 ];
				point[ 3 ] = triangulatedPoint[ i ][ 3 ];
				point = poseInv * point;

				size_t newPointId = map.addFeatureToKeyframe( mapFeat, meas, kId );
				_descriptors.addDescriptor( *triangulatedFeat[ i ], newPointId );
				_lastTrackedIds.push_back( newPointId );
				_lastTrackedFeatures.push_back( *triangulatedFeat[ i ] );
			}
			return _lastTrackedIds.size();
		} 
		return 0;
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
