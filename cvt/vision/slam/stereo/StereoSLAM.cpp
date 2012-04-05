/*
			CVT - Computer Vision Tools Library

 	 Copyright (c) 2012, Philipp Heise, Sebastian Klose

 	THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 	KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 	IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 	PARTICULAR PURPOSE.
 */
#include <cvt/vision/slam/stereo/StereoSLAM.h>

namespace cvt
{
	StereoSLAM::StereoSLAM( FeatureTracking* ft,
						   const CameraCalibration& c0,
						   size_t w0, size_t h0, 
						   const CameraCalibration& c1,
						   size_t w1, size_t h1 ):
		_camCalib0( c0 ), _camCalib1( c1 ),
		_featureTracking( ft ),
		_minTrackedFeatures( 50 ),
		_activeKF( -1 ),
		_minKeyframeDistance( 0.05 ),
		_maxKeyframeDistance( 0.5 )
	{
		// create the undistortion maps
		_undistortMap0.reallocate( w0, h0, IFormat::GRAYALPHA_FLOAT );
		Vector3f radial = c0.radialDistortion();
		Vector2f tangential = c0.tangentialDistortion();
		float fx = c0.intrinsics()[ 0 ][ 0 ];
		float fy = c0.intrinsics()[ 1 ][ 1 ];
		float cx = c0.intrinsics()[ 0 ][ 2 ];
		float cy = c0.intrinsics()[ 1 ][ 2 ];
		IWarp::warpUndistort( _undistortMap0, radial[ 0 ], radial[ 1 ], cx, cy, fx, fy, w0, h0, radial[ 2 ], tangential[ 0 ], tangential[ 1 ] );

		_undistortMap1.reallocate( w1, h1, IFormat::GRAYALPHA_FLOAT );
		radial = c1.radialDistortion();
		tangential = c1.tangentialDistortion();
		fx = c1.intrinsics()[ 0 ][ 0 ];
		fy = c1.intrinsics()[ 1 ][ 1 ];
		cx = c1.intrinsics()[ 0 ][ 2 ];
		cy = c1.intrinsics()[ 1 ][ 2 ];
		IWarp::warpUndistort( _undistortMap1, radial[ 0 ], radial[ 1 ], cx, cy, fx, fy, w1, h1, radial[ 2 ], tangential[ 0 ], tangential[ 1 ] );
		Eigen::Matrix3d K;
		EigenBridge::toEigen( K, _camCalib0.intrinsics() );
		_map.setIntrinsics( K );
	}

	void StereoSLAM::newImages( const Image& img0, const Image& img1 )
	{
		// undistort the first image
		IWarp::apply( _undist0, img0, _undistortMap0 );

		// predict current visible features by projecting with current estimate of pose
		std::vector<Vector2f> predictedPositions;
		std::vector<size_t>   predictedFeatureIds;
		_map.selectVisibleFeatures( predictedFeatureIds, predictedPositions,
									_pose.transformation(), _camCalib0, 3.0f /* TODO: make it a param */  );

		// track the predicted features
		PointSet2d p2d;
		std::vector<size_t>   trackedIds;
		_featureTracking->trackFeatures( p2d, trackedIds, 
										 predictedPositions, predictedFeatureIds, _undist0 );

		/* get the 3d points from the map */
		PointSet3d p3d;
		fillPointsetFromIds( p3d, trackedIds );

		numTrackedPoints.notify( p2d.size() );
		size_t numTrackedFeatures = p3d.size();
		if( numTrackedFeatures > 6 ){
			estimateCameraPose( p3d, p2d );
		} else {
			// too few features -> lost track: relocalization needed
		} 

		if( newKeyframeNeeded( numTrackedFeatures ) ){
			// undistort the second view
			IWarp::apply( _undist1, img1, _undistortMap1 );

			// wait until current ba thread is ready 
			if( _bundler.isRunning() ){
				_bundler.join();
			}

			// TODO: move the stereo correspondence and triangulation part into a seperate class
			//		 * add a StereoMatchingStrategy (e.g. based on orb, based on Patch block matching + klt?)
			size_t numNewPoints = 0;
			//size_t numNewPoints = _featureTracking->triangulateNewFeatures( _map, _pose, _undist0, _undist1 );

			//			Image debug;
			//			newStereoView.notify( debug );

			// new keyframe added -> run the sba thread	
			if( _map.numKeyframes() > 1 && numNewPoints ){
				_bundler.run( &_map );
				keyframeAdded.notify();
			
				mapChanged.notify( _map );
				std::cout << "New Keyframe Added" << std::endl;
			}
		}

		int last = _activeKF;
		_activeKF = _map.findClosestKeyframe( _pose.transformation() );
		if( _activeKF != last )
			std::cout << "Active KF: " << _activeKF << std::endl;
	}

	void StereoSLAM::estimateCameraPose( const PointSet3d & p3d, const PointSet2d & p2d )
	{
		const Matrix3f & kf = _camCalib0.intrinsics();
		Eigen::Matrix<double, 3, 3> Ke;
		Eigen::Matrix<double, 4, 4> extrC;
		EigenBridge::toEigen( Ke, kf );
		EigenBridge::toEigen( extrC, _camCalib0.extrinsics() );

		Eigen::Matrix4d me;
		/*
		   Matrix3d K;
		   EigenBridge::toCVT( K, kf );
		   Matrix4d m;
		   EPnPd epnp( p3d );
		   epnp.solve( m, p2d, K );

		// from EPnP we get the pose of the camera, to get pose of the rig, we need to remove the extrinsics
		EigenBridge::toEigen( me, m );
		me = extrC.inverse() * me;
		 */

		me = _pose.transformation();		

		PointCorrespondences3d2d<double> pointCorresp( Ke, extrC );
		pointCorresp.setPose( me );

		Eigen::Matrix<double, 3, 1> p3;
		Eigen::Matrix<double, 2, 1> p2;
		for( size_t i = 0; i < p3d.size(); i++ ){
			p3[ 0 ] = p3d[ i ].x;
			p3[ 1 ] = p3d[ i ].y;
			p3[ 2 ] = p3d[ i ].z;
			p2[ 0 ] = p2d[ i ].x;
			p2[ 1 ] = p2d[ i ].y;
			pointCorresp.add( p3, p2 );
		}

		RobustHuber<double, PointCorrespondences3d2d<double>::MeasType> costFunction( 2.0 );
		LevenbergMarquard<double> lm;
		TerminationCriteria<double> termCriteria( TERM_COSTS_THRESH | TERM_MAX_ITER );
		termCriteria.setCostThreshold( 0.01 );
		termCriteria.setMaxIterations( 20 );
		lm.optimize( pointCorresp, costFunction, termCriteria );

		//me = pointCorresp.pose().transformation().inverse();
		me = pointCorresp.pose().transformation();
		_pose.set( me );

		Matrix4f mf;
		EigenBridge::toCVT( mf, me );
		newCameraPose.notify( mf );
	}

	void StereoSLAM::clear()
	{
		if( _bundler.isRunning() )
			_bundler.join();

		_map.clear();
		_featureTracking->clear();

		Eigen::Matrix4d I( Eigen::Matrix4d::Identity() );
		_pose.set( I );
		_activeKF = -1;
	}

	void StereoSLAM::setPose( const Matrix4d& pose )
	{
		Eigen::Matrix4d pe;
		EigenBridge::toEigen( pe, pose );
		_pose.set( pe );
	}

	void StereoSLAM::debugPatchWorkImage( const std::set<size_t> & indices,
										  const std::vector<size_t> & featureIds,
										  const std::vector<FeatureMatch> & matches )
	{
		std::set<size_t>::const_iterator idIter = indices.begin();
		const std::set<size_t>::const_iterator idIterEnd = indices.end();

		size_t patchSize = 31;
		size_t patchHalf = patchSize >> 1;
		FeatureAnalyzer patchWork( 20, patchSize, 10 );

		Vector2f p0, p1;
		while( idIter != idIterEnd ){
			size_t featureId = featureIds[ *idIter ];
			const MapFeature & mf = _map.featureForId( featureId );
			const Keyframe & kf = _map.keyframeForId( *( mf.pointTrackBegin() ) );
			const MapMeasurement & meas = kf.measurementForId( featureId );

			p0.x = ( float )meas.point[ 0 ]; 
			p0.y = ( float )meas.point[ 1 ];
			if( p0.x < patchHalf || p0.y < patchHalf )
				std::cout << "Bad Point: " << p0  << " feature id: " << featureId << " kfId: " << kf.id() << std::endl;
			p0.x -= patchHalf;
			p0.y -= patchHalf;

			p1 = matches[ *idIter ].feature1->pt;
			p1.x -= patchHalf; 
			p1.y -= patchHalf;

			patchWork.addPatches( _undist0, p1, kf.image(), p0 );

			++idIter;
		}

		patchWork.image().save( "patchwork.png" );
	}

	bool StereoSLAM::newKeyframeNeeded( size_t numTrackedFeatures ) const
	{
		double kfDist = _minKeyframeDistance + 1.0;
		if( _activeKF != -1 ){
			kfDist = _map.keyframeForId( _activeKF ).distance( _pose.transformation() );
		}

		// if distance is too far from active, always create a new one:
		if( kfDist > _maxKeyframeDistance )
			return true;

		// if too few features and minimum distance from last keyframe create new
		if( numTrackedFeatures < _minTrackedFeatures /*&& kfDist > _minKeyframeDistance*/ )
			return true;

		return false;
	}

	void StereoSLAM::createDebugImageStereo( Image & debugImage, 
											const std::vector<FeatureMatch> & matches,
											const std::vector<size_t> & indices ) const
	{
		debugImage.reallocate( 2 * _undist0.width(), _undist0.height(), IFormat::RGBA_UINT8 );
		Image tmp( debugImage.width(), debugImage.height(), IFormat::GRAY_UINT8 );
		Recti rect( 0, 0, _undist0.width(), _undist0.height() );
		tmp.copyRect( 0, 0, _undist0, rect );
		tmp.copyRect( _undist0.width(), 0, _undist1, rect );

		tmp.convert( debugImage, IFormat::RGBA_UINT8 );
		{
			GFXEngineImage ge( debugImage );
			GFX g( &ge );

			g.color() = Color::RED;
			for( size_t i = 0; i < indices.size(); i++ ){
				const FeatureMatch & match = matches[ indices[ i ] ];
				Vector2f p2 = match.feature1->pt;
				p2.x += _undist0.width();
				g.drawLine( match.feature0->pt, p2 );
			}
		}
	}

	void StereoSLAM::fillPointsetFromIds( PointSet3d& pset, const std::vector<size_t>& ids ) const
	{
		std::vector<size_t>::const_iterator it = ids.begin();
		const std::vector<size_t>::const_iterator end = ids.end();
		while( it != end ){
			const Eigen::Vector4d& p = _map.featureForId( *it++ ).estimate();
			double n = 1.0 / p[ 3 ];	
			pset.add( Vector3d( p[ 0 ] * n, p[ 1 ] * n, p[ 2 ] * n  ) );
		}
	}
}
