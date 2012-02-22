/*
			CVT - Computer Vision Tools Library

 	 Copyright (c) 2012, Philipp Heise, Sebastian Klose

 	THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 	KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 	IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 	PARTICULAR PURPOSE.
 */
#include <cvt/vision/slam/stereo/FASTFeatureTracking.h>
#include <cvt/gfx/IMapScoped.h>

namespace cvt
{


	FASTFeatureTracking::FASTFeatureTracking( const CameraCalibration & c0, const CameraCalibration & c1 ) :
		_camCalib0( c0 ),
		_camCalib1( c1 ),
		_currentPyramid( new ImagePyramid( 3, 0.5f ) ),
		_lastPyramid( new ImagePyramid( 3, 0.5f ) ),
		_fastThreshold( 30 ),
		_fastBorder( 8 ),
		_windowRadius( 30 ),
		_patchSize( _fastBorder << 1 ),
		_sadThreshold( 0.7f ),
		_maxLineDist( 3.0f ),
		_maxTriangReprojError( 5.0f )
	{
		_fastContainer.resize( _currentPyramid->octaves() );
		_usedIdsForOctave.resize( _currentPyramid->octaves() );

		_scaleFactors.resize( _currentPyramid->octaves() );
		float invScale = 1.0f / _currentPyramid->scaleFactor(); 
		for( size_t i = 0; i < _scaleFactors.size(); i++ ){
			_scaleFactors[ i ] = Math::pow( invScale, i );  
		}

		// calc the fundamental matrix from the calibration data
		Vision::composeFundamental( _fundamental, 
								    c0.intrinsics(),
								    c0.extrinsics(),
								    c1.intrinsics(),
								   	c1.extrinsics() );
		_fundamental.transposeSelf();
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
        swapPyramids();

		// detect the features in the current image 
		img.convert( _debug, IFormat::RGBA_UINT8 );

		_currentPyramid->update( img );

		detectFeatures();

		trackSequential( p3d, p2d, map, img );
	
		// blue is detected
		for( size_t i = 0; i < _fastContainer.size(); i++ ){
			drawPointsInImage( _debug, Color::BLUE, _fastContainer[ i ] );
		}

		// green is tracked	
		drawPointsInImage( _debug, Color::GREEN, p2d );


	//	size_t offset = p2d.size();
		
	//	trackNonSequential( p3d, p2d, map, pose, img );
		
		debugImage.notify( _debug );
	}

	void FASTFeatureTracking::trackSequential( PointSet3d&		p3d, 
											   PointSet2d&		p2d,
											   const SlamMap&	map,
											   const Image&			)
	{
		// keep track of already assigned indices to avoid double associations
		Vector3d p3;
		Vector2d p2;

		size_t halfPatchSize = _patchSize >> 1;

		size_t savePos = 0;

		Feature2Df match;

		for( size_t i = 0; i < _lastTrackedFeatures.size(); i++ ){
			const Feature2Df& lastFeat = _lastTrackedFeatures[ i ];

			// search according to the stored octave
			IMapScoped<uint8_t> last(  ( *_lastPyramid)[ lastFeat.octave ] );
			IMapScoped<uint8_t> curr(  ( *_currentPyramid)[ lastFeat.octave ] );

			// get the pointer in the last image:
			const uint8_t* pLastUL = last.ptr() + ( int )( lastFeat.pt.y - halfPatchSize ) * last.stride() 
												+ ( int )( lastFeat.pt.x - halfPatchSize ); 

			float sad = findBestMatchInWindow( match, pLastUL, last.stride(), lastFeat );

			if( sad > _sadThreshold ){
				// got a match so add it to the point sets
				size_t currId = _lastTrackedIds[ i ];
				const MapFeature & mapFeat = map.featureForId( currId );
				p3.x = mapFeat.estimate().x(); 
				p3.y = mapFeat.estimate().y(); 
				p3.z = mapFeat.estimate().z();
				p3d.add( p3 );

				// scale the feature up to base level
				p2.x = match.pt.x * _scaleFactors[ match.octave ];
				p2.y = match.pt.y * _scaleFactors[ match.octave ];
				p2d.add( p2 );

				// TODO: this is a hack: fast feature id is the absolute position in the image array
				_usedIdsForOctave[ lastFeat.octave ].insert( ( size_t )( lastFeat.pt.y * last.stride() + lastFeat.pt.x ) );
				
				_lastTrackedIds[ savePos ] = currId;
				_lastTrackedFeatures[ savePos ] = match;
				savePos++;					
			}
		}

		// remove the lost features;
		_lastTrackedFeatures.erase( _lastTrackedFeatures.begin() + savePos, _lastTrackedFeatures.end() );
		_lastTrackedIds.erase( _lastTrackedIds.begin() + savePos, _lastTrackedIds.end() );
	}


	float FASTFeatureTracking::findBestMatchInWindow( Feature2Df& match, 
													  const uint8_t* last, size_t lstride, 
													  const Feature2Df& f ) const
	{
		size_t bestSAD	= 255 * Math::sqr( _patchSize );
		int x = f.pt.x;
		int y = f.pt.y;
		size_t line		= ( size_t )Math::max<int>( 0, x - _windowRadius );
		size_t stopLine = ( size_t )Math::min<int>( y + _windowRadius, ( *_lastPyramid )[ f.octave ].height() );
		size_t xStart	= ( size_t )Math::max<int>( 0, ( int )( x - _windowRadius ) );
		size_t xEnd		= ( size_t )Math::min<int>( x + _windowRadius, ( *_lastPyramid )[ f.octave ].width() );

		IMapScoped<uint8_t> curr( ( *_currentPyramid )[ f.octave ] );
		const std::set<size_t>::const_iterator idsEnd = _usedIdsForOctave[ f.octave ].end();
		
		// iterate over all current features of the octave within the window:
		while( line < stopLine ){
			FeatureRowLookupContainer::ConstRowIterator			it	= _fastContainer[ f.octave ].rowBegin( line ); 
			const FeatureRowLookupContainer::ConstRowIterator itEnd = _fastContainer[ f.octave ].rowEnd( line ); 

			while( it != itEnd && it->pt.x < xEnd ){
				if( it->pt.x >= xStart ){
					size_t fastId = ( size_t )( it->pt.y * curr.stride() + it->pt.x );
					if( _usedIdsForOctave[ f.octave ].find( fastId ) != idsEnd ){
						it++;
						continue;
					} 

					// get the pointer to the upper left of the current octave image
					const uint8_t* cPtr = curr.ptr() + ( int )( it->pt.y - _fastBorder ) * curr.stride() + 
						( int )it->pt.x - _fastBorder;

					size_t currSAD = patchSAD( cPtr, curr.stride(), last, lstride );

					if( currSAD < bestSAD ){
						bestSAD = currSAD;
						match = *it;
					}
				}
				it++;
			}
			line++;
		}

		// normalize the SAD: 1.0f -> perfect match 
		return 1.0f - bestSAD / ( 255.0f * Math::sqr( _patchSize ) );
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

	size_t FASTFeatureTracking::triangulateNewFeatures( SlamMap& map,
														const SE3<double> & pose,
														const Image & firstView,
														const Image & secondView )
	{
		ImagePyramid pyramid( _currentPyramid->octaves(), _currentPyramid->scaleFactor() );
		pyramid.update( secondView );

		// start from the highest level:
		Vector2f p0, p1;
		Vector3f p3;
		p3.z = 1.0f;
		int currOctave = pyramid.octaves() - 1;
		Feature2Df match;

		std::vector<Vector4f> points3d;
		std::vector<Feature2Df> firstViewMatches;

		while( currOctave >= 0 ){
			std::vector<Feature2Df> octaveFeatures;
			VectorFeature2DInserter<float> inserter( octaveFeatures );
			
			// detect fast features: 
			FAST::detect9( pyramid[ currOctave ], _fastThreshold, inserter, _fastBorder );

			// TODO: nonmax supression?!

			IMapScoped<uint8_t> firstView( ( *_currentPyramid )[ currOctave ] );
			IMapScoped<uint8_t> i2( pyramid[ currOctave ] );

			for( size_t r = 0; r < octaveFeatures.size(); r++ ){
				p3.x = octaveFeatures[ r ].pt.x;
				p3.y = octaveFeatures[ r ].pt.y;
				Line2Df epiline( _fundamental * p3 );

				// also scale the epiline ...
				epiline.vector()[ 2 ] /= _scaleFactors[ currOctave ];

				// this is the patch in the second image
				const uint8_t* patchPtr = i2.ptr() + 
										( ( int )p3.y - _fastBorder ) * i2.stride() + 
										  ( int )p3.x - _fastBorder;

				float sad = findBestMatchOnLine( match, epiline, currOctave,
												 firstView.ptr(), firstView.stride(),
												 patchPtr, i2.stride() );

				if( sad > _sadThreshold ){
					// seems to be a match: triangulate a 3d point and check the result

					// scale the matched pts to level 0
					p0 = match.pt * _scaleFactors[ currOctave ];
					p1 = octaveFeatures[ r ].pt * _scaleFactors[ currOctave ];

					if( !alreadyTrackingFeatureInRange( match, _patchSize ) ){
						Vector4f pnew;
						float err = triangulate( p0, p1, pnew );

						if( err < _maxTriangReprojError ){
							// we have a new 3D point
							size_t fastId = match.pt.y * firstView.stride() + match.pt.x;
							_usedIdsForOctave[ currOctave ].insert( fastId );
							points3d.push_back( pnew );
							firstViewMatches.push_back( match );
						}
					}
				}

			}

			currOctave--;
		}


		if( points3d.size() > 10 /* TODO: make this a parameter? */ ){

			// add a new keyframe to the map
			size_t kId = map.addKeyframe( pose.transformation() );
			Keyframe & keyframe = map.keyframeForId( kId );
			keyframe.setImage( firstView );

			MapMeasurement meas;

			Eigen::Matrix4d featureCov = Eigen::Matrix4d::Identity();
			MapFeature mapFeat( Eigen::Vector4d::Zero(), featureCov );

			// add the currently tracked features to the keyframe!
			for( size_t i = 0; i < _lastTrackedIds.size(); i++ ){				
				size_t pointId = _lastTrackedIds[ i ];
				float s = _scaleFactors[ _lastTrackedFeatures[ i ].octave ];
				meas.point[ 0 ] = _lastTrackedFeatures[ i ].pt.x * s;
				meas.point[ 1 ] = _lastTrackedFeatures[ i ].pt.y * s;
				map.addMeasurement( pointId, kId, meas );
			}

			Eigen::Matrix4d poseInv = pose.transformation().inverse();
			for( size_t i = 0; i < points3d.size(); i++ ){
				meas.point[ 0 ] = firstViewMatches[ i ].pt.x;
				meas.point[ 1 ] = firstViewMatches[ i ].pt.y;

				Eigen::Vector4d & point = mapFeat.estimate();
				point[ 0 ] = points3d[ i ][ 0 ];
				point[ 1 ] = points3d[ i ][ 1 ];
				point[ 2 ] = points3d[ i ][ 2 ];
				point[ 3 ] = points3d[ i ][ 3 ];
				point = poseInv * point;

				size_t newPointId = map.addFeatureToKeyframe( mapFeat, meas, kId );
				_lastTrackedIds.push_back( newPointId );
				_lastTrackedFeatures.push_back( firstViewMatches[ i ] );
			}
			return points3d.size();
		}
		return 0;
	}


	float FASTFeatureTracking::triangulate( Vector2f& p0, Vector2f& p1, Vector4f& ptriang ) const
	{
		Vision::correctCorrespondencesSampson( p1, p0, _fundamental );
		return triangulateSinglePoint( ptriang, p0, p1, _camCalib0.projectionMatrix(), _camCalib1.projectionMatrix() );
	}
			
	float FASTFeatureTracking::findBestMatchOnLine( Feature2Df& match, 
							   const Line2Df& epiline,
							   size_t octave,
							   const uint8_t* ptrA, size_t strideA,
							   const uint8_t* patch, size_t patchStride ) const
	{
		size_t bestSAD = 255 * Math::sqr( _patchSize );
							   
		const FeatureRowLookupContainer& container = _fastContainer[ octave ];
		size_t w = ( *_currentPyramid )[ octave ].width();
		size_t h = ( *_currentPyramid )[ octave ].height();

		// only check those lines affected:
        int l0 = -epiline[ 2 ] / epiline[ 1 ];
        int l1 = ( -epiline[ 2 ] - epiline[ 0 ] * w ) / epiline[ 1 ];        
        size_t lStart = Math::clamp<int>( l0, 0, h-1 );
        size_t lStop  = Math::clamp<int>( l1, 0, h-1 );
        
        if( lStart > lStop ){
            size_t tmp = lStart;
            lStart = lStop;
            lStop  = tmp;
        }
        
		const std::set<size_t>::const_iterator idsEnd = _usedIdsForOctave[ octave ].end();

		// check those lines:
		while( lStart <= lStop ){
			FeatureRowLookupContainer::ConstRowIterator r = container.rowBegin( lStart );
			const FeatureRowLookupContainer::ConstRowIterator rEnd = container.rowEnd( lStart );

			while( r != rEnd ){
				// check if the feature lies "close" to the line:
				if( Math::abs( epiline.distance( r->pt ) ) < _maxLineDist ){

					size_t fastId = ( size_t )( r->pt.y * strideA + r->pt.x );
					if( _usedIdsForOctave[ octave ].find( fastId ) == idsEnd ){
						// check if the SAD is lower than the current best:
						const uint8_t* p = ptrA + strideA * ( int )( r->pt.y - _fastBorder ) +
										   ( int )r->pt.x - _fastBorder;
						size_t sad = patchSAD( p, strideA, patch, patchStride );

						if( sad < bestSAD ){
							bestSAD = sad;
							match = *r;
						}
					} 
				}

				r++;
			}

			lStart++;
		}

		return 1.0f - bestSAD / ( 255.0f * Math::sqr( _patchSize ) );
	}


	size_t FASTFeatureTracking::patchSAD( const uint8_t* ptrA, size_t strideA,
										  const uint8_t* ptrB, size_t strideB ) const
	{
		SIMD* simd = SIMD::instance();
		size_t pSize = _patchSize;
		size_t currSAD = 0;
		while( pSize-- ){
			currSAD += simd->SAD( ptrA, ptrB, _patchSize );
			ptrA += strideA;
			ptrB += strideB;
		}
		return currSAD;
	}


	bool FASTFeatureTracking::alreadyTrackingFeatureInRange( const Feature2Df& f, float rangeSqr ) const
	{
		Vector2f f0 = f.pt * _scaleFactors[ f.octave ];
		Vector2f f1;
		
		for( size_t i = 0; i < _lastTrackedFeatures.size(); i++ ){
			f1 = _lastTrackedFeatures[ i ].pt * _scaleFactors[ f.octave ];
			if( ( f1 - f0 ).lengthSqr() < rangeSqr )
				return true;
		}
		return false;
	}

	void FASTFeatureTracking::clear()
	{
		_lastTrackedIds.clear();
		_lastTrackedFeatures.clear();
	}


	void FASTFeatureTracking::detectFeatures()
	{
		ImagePyramid& pyr = *_currentPyramid;
        Image debug;
        pyr[ 0 ].convert( debug, IFormat::RGBA_UINT8 );
		for( size_t i = 0; i < _currentPyramid->octaves(); i++ ){
			_fastContainer[ i ].setNumRows( pyr[ i ].height() );
			_fastContainer[ i ].setOctave( i );
			_fastContainer[ i ].clear();
			_usedIdsForOctave[ i ].clear();
			FAST::detect9( pyr[ i ], _fastThreshold, _fastContainer[ i ], _fastBorder );
            drawPointsInImage( debug, cvt::Color::GREEN, _fastContainer[ i ] );
        }
        drawPointsInImage( debug, cvt::Color::RED, _lastTrackedFeatures );
        debug.save( "fast_tracking_currg_lastr.png" );
	}


	void FASTFeatureTracking::swapPyramids()
	{
		ImagePyramid* tmp = _currentPyramid;
		_currentPyramid = _lastPyramid;
		_lastPyramid = tmp;	
	}

		
}
