#include <cvt/vision/slam/stereo/FASTFeatureTracking.h>

namespace cvt
{
	FASTFeatureTracking::FASTFeatureTracking( const CameraCalibration & c0, const CameraCalibration & c1 ) :
		_camCalib0( c0 ),
		_camCalib1( c1 ),
		_currentPyramid( new ImagePyramid( 3, 0.5f ) ),
		_lastPyramid( new ImagePyramid( 3, 0.5f ) ),
		_fastThreshold( 20 ),
		_fastBorder( 8 ),
		_windowRadius( 20 )
	{
		_fastContainer.resize( _currentPyramid->octaves() );
		_usedIdsForOctave.resize( _currentPyramid->octaves() );
	}

	FASTFeatureTracking::~FASTFeatureTracking()
	{
	}

	void FASTFeatureTracking::trackFeatures( PointSet3d&			p3d, 
											 PointSet2d&			p2d,
											 const SlamMap&			map,
											 const SE3<double>&		pose,
											 const Image&			img )
	{
		// detect the features in the current image 
		img.convert( _debug, IFormat::RGBA_UINT8 );

		_currentPyramid->update( img );

		detectFeatures();

		trackSequential( p3d, p2d, map, img );
		
		// drawPointsInImage( _debug, Color::GREEN, p2d );

		size_t offset = p2d.size();
		trackNonSequential( p3d, p2d, map, pose, img );
		
		debugImage.notify( _debug );
	}

	void FASTFeatureTracking::trackSequential( PointSet3d&		p3d, 
											   PointSet2d&		p2d,
											   const SlamMap&	map,
											   const Image&		img )
	{
		// keep track of already assigned indices to avoid double associations
		Vector3d p3;
		Vector2d p2;

		size_t savePos = 0;
		for( size_t i = 0; i < _lastTrackedFeatures.size(); i++ ){
			
			

			// TODO: 
			std::cerr << "TODO: FAST MATCHING IN WINDOW" << std::endl;

			/*
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
			*/
		}

		_lastTrackedFeatures.erase( _lastTrackedFeatures.begin() + savePos, _lastTrackedFeatures.end() );
		_lastTrackedIds.erase( _lastTrackedIds.begin() + savePos, _lastTrackedIds.end() );
	}


	void FASTFeatureTracking::trackNonSequential( PointSet3d&			p3d, 
												  PointSet2d&			p2d,
												  const SlamMap&		map,
												  const SE3<double>&	pose,
												  const Image&			img )
	{
		// predict visible features with map and current pose
		_predictedIds.clear();
		_predictedPositions.clear();
		map.selectVisibleFeatures( _predictedIds, _predictedPositions, pose.transformation(), _camCalib0 );

		//drawPointsInImage( _debug, Color::YELLOW, _predictedPositions );

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

		// we want to find the best matching feature from current, that lies
		// within a certain distance from the "predicted" position, if present...
		std::vector<size_t>::const_iterator currentId = _predictedIds.begin();
		std::vector<size_t>::const_iterator tEnd	  = _predictedIds.end();
		std::vector<Vector2f>::const_iterator pred	  = _predictedPositions.begin();

		Vector3d p3;
		Vector2d p2;
		while( currentId != tEnd ){
			//if( m.feature1 ){
			//	// got a match so add it to the point sets
			//	const MapFeature & mapFeat = map.featureForId( *currentId );
			//	size_t keyframeId = *( mapFeat.pointTrackBegin() );
			//	if( checkFeatureSAD( m.feature0->pt, m.feature1->pt, 
			//						map.keyframeForId( keyframeId ).image(), img ) ) {
			//		p3.x = mapFeat.estimate().x(); 
			//		p3.y = mapFeat.estimate().y(); 
			//		p3.z = mapFeat.estimate().z();
			//		p3d.add( p3 );
			//		p2.x = m.feature1->pt.x;
			//		p2.y = m.feature1->pt.y;
			//		p2d.add( p2 );
			//		_orb0MatchedIds.insert( orbid );
			//		_lastTrackedIds.push_back( *currentId );
			//		_lastTrackedFeatures.push_back( *( ORBFeature* )m.feature1 );
			//	}
			//}
			++currentId;
			++pred;
		}
	}

	/*
	int FASTFeatureTracking::matchInWindow( const Vector2f & p, std::set<size_t> & used ) const
	{
		return -1;
	}
	*/

	size_t FASTFeatureTracking::triangulateNewFeatures( SlamMap& map,
														 const SE3<double> & pose,
														 const Image & firstView,
														 const Image & secondView )
	{
		/*
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
	*/
		return 0;
	}

	void FASTFeatureTracking::clear()
	{
		_lastTrackedIds.clear();
		_lastTrackedFeatures.clear();
	}


	void FASTFeatureTracking::detectFeatures()
	{
		ImagePyramid& pyr = *_currentPyramid;
		for( size_t i = 0; i < _currentPyramid->octaves(); i++ ){
			_fastContainer[ i ].setNumRows( pyr[ i ].height() );
			FAST::detect9( pyr[ i ], _fastThreshold, _fastContainer[ i ], _fastBorder );
		}
	}


	void FASTFeatureTracking::swapPyramids()
	{
		ImagePyramid* tmp = _currentPyramid;
		_currentPyramid = _lastPyramid;
		_lastPyramid = tmp;	
	}

}
