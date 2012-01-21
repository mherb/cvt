#ifndef CVT_STEREO_SLAM_H
#define CVT_STEREO_SLAM_H

#include <cvt/vision/CameraCalibration.h>
#include <cvt/vision/PointCorrespondences3d2d.h>
#include <cvt/gfx/ifilter/IWarp.h>
#include <cvt/gfx/GFXEngineImage.h>
#include <cvt/vision/Vision.h>
#include <cvt/vision/EPnP.h>
#include <cvt/vision/FeatureMatch.h>
#include <cvt/math/SE3.h>
#include <cvt/math/sac/RANSAC.h>
#include <cvt/math/sac/EPnPSAC.h>
#include <cvt/math/LevenbergMarquard.h>
#include <cvt/util/Signal.h>
#include <cvt/util/Time.h>
#include <cvt/util/Time.h>

#include <cvt/vision/slam/SlamMap.h>
#include <cvt/vision/slam/Keyframe.h>

#include "ORBStereoMatching.h"
#include "DescriptorDatabase.h"
#include "ORBTracking.h"
#include "MapOptimizer.h"
#include "FeatureAnalyzer.h"

#include <set>

namespace cvt
{
	// Managing class for stereo SLAM
	class StereoSLAM
	{
		public:
			StereoSLAM( FeatureTracking* ft, 
					    const CameraCalibration& c0, size_t w0, size_t h0,
						const CameraCalibration& c1, size_t w1, size_t h1 );

			// new round with two new images, maybe also hand in a pose prediction?
			void newImages( const Image& img0, const Image& img1 );

			const Image& undistorted( size_t idx = 0 ) const 
			{
				if( idx )
					return _undist1;
				return _undist0;
			}

			const SlamMap & map() const { return _map; }
			void clear();

			Signal<const Image&>		newStereoView;	
			Signal<const SlamMap&>		mapChanged;	
			Signal<const Matrix4f&>		newCameraPose;	
			Signal<const Image&>		trackedPointsImage;	
			Signal<size_t>				numTrackedPoints;	

		private:
			/* camera calibration data and undistortion maps */
			CameraCalibration	_camCalib0;
			CameraCalibration	_camCalib1;
			Image				_undistortMap0;
			Image				_undistortMap1;

			/* undistorted images */
			Image				_undist0;
			Image				_undist1;


			// FeatureTracker (single view)
			float				_trackingSearchRadius;
			FeatureTracking*	_featureTracking;

			// minimum needed features before new keyframe is added
			size_t				_minTrackedFeatures;


			/* the current pose of the camera rig */
			SE3<double>			_pose;

			/* the active Keyframe Id (closest to _pose) */
			int					_activeKF;
			double				_minKeyframeDistance;
			double				_maxKeyframeDistance;

			SlamMap				_map;

			MapOptimizer		_bundler;
			Image				_lastImage;

			void estimateCameraPose( const PointSet3d & p3d, const PointSet2d & p2d );

			void debugPatchWorkImage( const std::set<size_t> & indices,
								      const std::vector<size_t> & featureIds,
								      const std::vector<FeatureMatch> & matches );

			bool newKeyframeNeeded( size_t numTrackedFeatures ) const;

			void createDebugImageMono( Image & debugImage, const PointSet2d & tracked ) const;
			void createDebugImageStereo( Image & debugImage, 
										 const std::vector<FeatureMatch> & matches,
										 const std::vector<size_t> & indices ) const;
	};

	inline StereoSLAM::StereoSLAM( FeatureTracking* ft,
								   const CameraCalibration& c0,
								   size_t w0, size_t h0, 
								   const CameraCalibration& c1,
								   size_t w1, size_t h1 ):
		_camCalib0( c0 ), _camCalib1( c1 ),
		_featureTracking( ft ),
		_minTrackedFeatures( 50 ),
		_activeKF( -1 ),
		_minKeyframeDistance( 0.05 ),
		_maxKeyframeDistance( 1.0 )
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

	inline void StereoSLAM::newImages( const Image& img0, const Image& img1 )
	{
		// undistort the first image
		IWarp::apply( _undist0, img0, _undistortMap0 );

		PointSet2d p2d;
		PointSet3d p3d;
		_featureTracking->trackFeatures( p3d, p2d, _map, _pose, _undist0 );

		Image debug;
		createDebugImageMono( debug, p2d );
		trackedPointsImage.notify( debug );
		numTrackedPoints.notify( p2d.size() );

		size_t numTrackedFeatures = p3d.size();
		if( numTrackedFeatures > 6 ){
			estimateCameraPose( p3d, p2d );
		} 

		if( newKeyframeNeeded( numTrackedFeatures ) ){
			std::cout << "Adding new keyframe: currently tracked features -> " << p3d.size() << std::endl;
			IWarp::apply( _undist1, img1, _undistortMap1 );

			if( _bundler.isRunning() ){
				std::cout << "Waiting for last SBA to finish!" << std::endl;
				_bundler.join();
			}

			// TODO: call the init method
			size_t numNewPoints = _featureTracking->triangulateNewFeatures( _map, _pose, _undist0, _undist1 );	
			std::cout << "Added " << numNewPoints << " new 3D Points" << std::endl;
			Image debug;
//			createDebugImageStereo( debug, _featureTracking.lastStereoMatches(), matchedStereoIndices );
//			newStereoView.notify( debug );

			// new keyframe added -> run the sba thread	
			if( _map.numKeyframes() > 1 ){
				_bundler.run( &_map );
				//_bundler.join();
			}
			mapChanged.notify( _map );				
		}
 
		int last = _activeKF;
		_activeKF = _map.findClosestKeyframe( _pose.transformation() );
		if( _activeKF != last )
			std::cout << "Active KF: " << _activeKF << std::endl;

		
	}

	inline void StereoSLAM::estimateCameraPose( const PointSet3d & p3d, const PointSet2d & p2d )
	{
		Matrix3d K;
		const Matrix3f & kf = _camCalib0.intrinsics();
		K[ 0 ][ 0 ] = kf[ 0 ][ 0 ];	K[ 0 ][ 1 ] = kf[ 0 ][ 1 ]; K[ 0 ][ 2 ] = kf[ 0 ][ 2 ];
		K[ 1 ][ 0 ] = kf[ 1 ][ 0 ]; K[ 1 ][ 1 ] = kf[ 1 ][ 1 ]; K[ 1 ][ 2 ] = kf[ 1 ][ 2 ];
		K[ 2 ][ 0 ] = kf[ 2 ][ 0 ]; K[ 2 ][ 1 ] = kf[ 2 ][ 1 ]; K[ 2 ][ 2 ] = kf[ 2 ][ 2 ];
		Eigen::Matrix<double, 3, 3> Ke;
		Eigen::Matrix<double, 4, 4> extrC;
		EigenBridge::toEigen( Ke, K );
		EigenBridge::toEigen( extrC, _camCalib0.extrinsics() );
		
		Eigen::Matrix4d me;
		/*
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
		termCriteria.setMaxIterations( 10 );
		lm.optimize( pointCorresp, costFunction, termCriteria );

		//me = pointCorresp.pose().transformation().inverse();
		me = pointCorresp.pose().transformation();
		_pose.set( me );

		Matrix4f mf;
		EigenBridge::toCVT( mf, me );
		newCameraPose.notify( mf );
	}

	inline void StereoSLAM::clear()
	{
		if( _bundler.isRunning() )
			_bundler.join();

		_map.clear();
		_featureTracking->clear();

		Eigen::Matrix4d I( Eigen::Matrix4d::Identity() );
		_pose.set( I );
		_activeKF = -1;
	}


			
	inline void StereoSLAM::debugPatchWorkImage( const std::set<size_t> & indices,
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

	inline bool StereoSLAM::newKeyframeNeeded( size_t numTrackedFeatures ) const
	{
		double kfDist = _minKeyframeDistance + 1.0;
	    if( _activeKF != -1 ){
			kfDist = _map.keyframeForId( _activeKF ).distance( _pose.transformation() );
		}

		// if distance is too far from active, always create a new one:
		if( kfDist > _maxKeyframeDistance )
			return true;

		// if too few features and minimum distance from last keyframe create new
		if( numTrackedFeatures < _minTrackedFeatures && kfDist > _minKeyframeDistance )
			return true;

		return false;
	}

	inline void StereoSLAM::createDebugImageMono( Image & debug, const PointSet2d & tracked ) const 
	{
		_undist0.convert( debug, IFormat::RGBA_UINT8 );

		{
			GFXEngineImage ge( debug );
			GFX g( &ge );
			g.color() = Color::GREEN;

			Recti r;
			size_t pSize = 17;
			size_t phSize = pSize >> 1;
			r.width = pSize;
			r.height = pSize;
			for( size_t i = 0; i < tracked.size(); i++ ){
				r.x = ( int )tracked[ i ].x - phSize;
				r.y = ( int )tracked[ i ].y - phSize;
				g.drawRect( r );
			}
		}
	}
	
	inline void StereoSLAM::createDebugImageStereo( Image & debugImage, 
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
}

#endif
