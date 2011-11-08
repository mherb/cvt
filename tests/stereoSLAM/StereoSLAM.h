#ifndef CVT_STEREO_SLAM_H
#define CVT_STEREO_SLAM_H

#include <cvt/vision/CameraCalibration.h>
#include <cvt/vision/PointCorrespondences3d2d.h>
#include <cvt/gfx/ifilter/IWarp.h>
#include <cvt/vision/Vision.h>
#include <cvt/vision/ORB.h>
#include <cvt/vision/EPnP.h>
#include <cvt/vision/FeatureMatch.h>
#include <cvt/math/SE3.h>
#include <cvt/math/sac/RANSAC.h>
#include <cvt/math/sac/EPnPSAC.h>
#include <cvt/math/LevenbergMarquard.h>
#include <cvt/util/Signal.h>

#include "ORBStereoMatching.h"
#include "FeatureTracking.h"
#include "Map.h"

#include <set>

namespace cvt
{
	// Managing class for stereo SLAM
	class StereoSLAM
	{
		public:

			struct ORBData
			{
				ORB* orb0;
				Image* img0;
				ORB* orb1;
				Image* img1;
				std::vector<FeatureMatch>* matches;
			};

			StereoSLAM( const CameraCalibration& c0,
						size_t w0, size_t h0,
						const CameraCalibration& c1,
						size_t w1, size_t h1 );

			// new round with two new images, maybe also hand in a pose prediction?
			void newImages( const Image& img0, const Image& img1 );

			const Image& undistorted( size_t idx = 0 ) const 
			{
				if( idx )
					return _undist1;
				return _undist0;
			}

			Signal<const ORBData*>		newORBData;	
			Signal<const Keyframe&>		newKeyFrame;	
			Signal<const std::vector<Vector4f>&>		newPoints;	
			Signal<const Matrix4f&>		newCameraPose;	

		private:
			/* camera calibration data and undistortion maps */
			CameraCalibration	_camCalib0;
			CameraCalibration	_camCalib1;
			Image				_undistortMap0;
			Image				_undistortMap1;

			/* undistorted images */
			Image				_undist0;
			Image				_undist1;
			
			/* descriptor matching parameters */
			float				_matcherMaxLineDistance;	
			float				_matcherMaxDescriptorDist;
			ORBStereoMatching	_stereoMatcher;
			float				_maxTriangReprojError;

			/* orb parameters */
			size_t				_orbOctaves;
			float				_orbScaleFactor;
			uint8_t				_orbCornerThreshold;
			size_t				_orbMaxFeatures;
			bool				_orbNonMaxSuppression;

			// FeatureTracker (single view)
			float				_trackingSearchRadius;
		    FeatureTracking		_featureTracking;

			/* the current (camera) pose */
			SE3<double>			_pose;

			/* the active Keyframe (closest to _pose) */
			Keyframe*			_activeKF;

			Map					_map;

			// triangulate 3D points from 2D matches
			float triangulate( MapFeature* & feature, FeatureMatch & match, size_t keyframeId ) const;

			// using the current camera pose, predict the visible features + descriptors
			void predictVisibleFeatures( std::vector<MapFeature*> & features, 
										 std::vector<Vector2f> & predictedPos,
										 std::vector<ORBFeature*> & descriptor ) const;

			void estimateCameraPose( const PointSet3d & p3d, const PointSet2d & p2d );
	};

	inline StereoSLAM::StereoSLAM( const CameraCalibration& c0,
								   size_t w0, size_t h0, 
								   const CameraCalibration& c1,
								   size_t w1, size_t h1 ):
		_camCalib0( c0 ), _camCalib1( c1 ),
		_matcherMaxLineDistance( 5.0f ),
		_matcherMaxDescriptorDist( 70 ),
		_stereoMatcher( _matcherMaxLineDistance, _matcherMaxDescriptorDist, _camCalib0, _camCalib1 ),
		_maxTriangReprojError( 5.0f ),
		_orbOctaves( 4 ), 
		_orbScaleFactor( 0.5f ),
		_orbCornerThreshold( 20 ),
		_orbMaxFeatures( 2000 ),
		_orbNonMaxSuppression( true ),
		_trackingSearchRadius( 40.0f ),
		_featureTracking( _matcherMaxDescriptorDist, _trackingSearchRadius ),
		_activeKF( 0 )
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
	}

	inline void StereoSLAM::newImages( const Image& img0, const Image& img1 )
	{
		// undistort
		IWarp::apply( _undist0, img0, _undistortMap0 );

		// create the ORB
		ORB orb0( _undist0, 
				  _orbOctaves, 
				  _orbScaleFactor,
				  _orbCornerThreshold,
				  _orbMaxFeatures,
				  _orbNonMaxSuppression );

		// predict visible features with map and current pose
		std::vector<MapFeature*> mapFeatures;
		std::vector<Vector2f> predictedPositions;
		std::vector<ORBFeature*> descriptors;
		predictVisibleFeatures( mapFeatures, predictedPositions, descriptors );

		std::vector<FeatureMatch> matchedFeatures;
		_featureTracking.trackFeatures( matchedFeatures, 
									    descriptors,
									    predictedPositions,
									    orb0 );

		PointSet2d p2d;
		PointSet3d p3d;
		Vector3d p3;
		Vector2d p2;

		for( size_t i = 0; i < matchedFeatures.size(); i++ ){
			if( matchedFeatures[ i ].feature1 ){
				// got a match so add it to the point sets
				p3.x = mapFeatures[ i ]->worldData().x(); 
				p3.y = mapFeatures[ i ]->worldData().y(); 
				p3.z = mapFeatures[ i ]->worldData().z();
				p3d.add( p3 );
				p2.x = matchedFeatures[ i ].feature1->pt.x;
				p2.y = matchedFeatures[ i ].feature1->pt.y;
				p2d.add( p2 );
			}
		}

		/* at least 10 corresp. */
		if( p3d.size() > 9 ){
			estimateCameraPose( p3d, p2d );
		} else {
			std::cout << "TOO FEW FEATURES TO ESTIMATE POSE" << std::endl;
			// TODO: How do we adress this? 
			// - Create a submap and try to merge it later
			// - relocalize against whole map?
		}

//		std::cout << "Tracked features " << p3d.size() << std::endl;

		if( p3d.size() < 200 ){
			IWarp::apply( _undist1, img1, _undistortMap1 );

			// create the ORB
			ORB orb1(_undist1, 
					 _orbOctaves, 
					 _orbScaleFactor,
					 _orbCornerThreshold,
					 _orbMaxFeatures,
					 _orbNonMaxSuppression );

			// find stereoMatches
			std::vector<FeatureMatch> matches;
			_stereoMatcher.matchEpipolar( matches, orb0, orb1 );

			// Create a new keyframe with image 0 as reference image
			if( matches.size() > 0 ){
				std::vector<Vector4f> pts4f;
				Vector4f currP;
				
				//std::cout << "Tracked Features: " << p3d.size();
				
				Keyframe * kf = new Keyframe( _undist0 );
				kf->setPose( _pose.transformation() );
				size_t kId = kf->id();
			
				for( size_t i = 0; i < matches.size(); i++ ){
					MapFeature* mapFeat = 0;
					if( matches[ i ].feature1 ){
						float reprErr = triangulate( mapFeat, matches[ i ], kId );
						
						if( reprErr < _maxTriangReprojError ){
							const Eigen::Vector4d & wp = mapFeat->worldData();
							currP.x = ( float ) ( wp.x() );
							currP.y = ( float ) ( wp.y() );
							currP.z = ( float ) ( wp.z() );
							currP.w = ( float ) ( wp.w() );
							pts4f.push_back( currP );
							kf->addFeature( mapFeat );
						}
					}
				}
				newPoints.notify( pts4f );

				// TODO: add the currently tracked MapFeatures to the Keyframe as well!
				// in order to get the dependency between the frames

				// TODO: make sure the keyframe has mapfeatures!
				_map.addKeyframe( kf );
				newKeyFrame.notify( *kf );

				_activeKF = _map.selectClosestKeyframe( _pose.transformation() );
			} else {
				// WHAT DO WE DO IF WE CAN'T FIND STEREO CORRESPONDENCES?!
			}

			// notify observers that there is new orb data
			// e.g. gui or a localizer 
			ORBData data;
			data.orb0 = &orb0;
			data.img0 = &_undist0;
			data.orb1 = &orb1;
			data.img1 = &_undist1;
			data.matches = &matches;
			newORBData.notify( &data );
		}
	}

	inline float StereoSLAM::triangulate( MapFeature* & feature, FeatureMatch & match, size_t keyframeId ) const
	{
		Vector4f tmp;
		Vector4f repr;
		Vector2f repr2, p0, p1;

		p0 = match.feature0->pt;
		p1 = match.feature1->pt;

		Vision::correctCorrespondencesSampson( p0, 
											   p1, 
											  _stereoMatcher.fundamental() );

		Vision::triangulate( tmp,
							_camCalib0.projectionMatrix(),
							_camCalib1.projectionMatrix(),
							p0,
							p1 );
			
		// normalize 4th coord;
		tmp /= tmp.w;

		if( tmp.z > 0.0f ){
			float error = 0.0f;

			repr = _camCalib0.projectionMatrix() * tmp;
			repr2.x = repr.x / repr.z;
			repr2.y = repr.y / repr.z;

			error += ( match.feature0->pt - repr2 ).length();

			repr = _camCalib1.projectionMatrix() * tmp;
			repr2.x = repr.x / repr.z;
			repr2.y = repr.y / repr.z;
			error += ( match.feature1->pt - repr2 ).length();

			error /= 2.0f;

			if( error < _maxTriangReprojError ){
				Eigen::Vector4d np( tmp.x, tmp.y, tmp.z, tmp.w );

				// transform to world coordinates
				np = _pose.transformation() * np;
			
				feature = new MapFeature( np );
				feature->addMeasurement( keyframeId, *( (ORBFeature*)match.feature0 ) );	
			}
			return error;
		} 
		
		return _maxTriangReprojError;
	}

	inline void StereoSLAM::predictVisibleFeatures( std::vector<MapFeature*> & features, 
													std::vector<Vector2f> & predictedPos,
													std::vector<ORBFeature*> & descriptor ) const
	{
		const Eigen::Matrix4d & pose = _pose.transformation();
		Eigen::Matrix4d poseInv = pose.inverse();

		size_t w = _undist0.width(); 
		size_t h = _undist0.height(); 
		
		// get keyframes of interest
		std::vector<Keyframe*> keyframes;
		double maxKeyframeDistance = 50.0;
		_map.selectNearbyKeyframes( keyframes, pose, maxKeyframeDistance );

		std::set<size_t> usedPoints;

		Eigen::Vector4d pointInCam;
		Vector4f pic, sp;
		Vector2f pointInScreen;
		for( size_t i = 0; i < keyframes.size(); i++ ){
			Keyframe* kf = keyframes[ i ];
			for( size_t p = 0; p < kf->numFeatures(); p++ ){
				// project the feature onto the current camera
				MapFeature* feature = kf->feature( p );
				if( usedPoints.find( feature->id() ) == usedPoints.end() ){
					pointInCam = poseInv * feature->worldData();
					pointInCam /= pointInCam[ 3 ];
					if( pointInCam[ 2 ] > 0.0 ){
						pic[ 0 ] = ( float )pointInCam[ 0 ];
						pic[ 1 ] = ( float )pointInCam[ 1 ];
						pic[ 2 ] = ( float )pointInCam[ 2 ];
						pic[ 3 ] = ( float )pointInCam[ 3 ];

						// project it to the screen:
						sp = _camCalib0.projectionMatrix() * pic;
						pointInScreen.x = sp.x / sp.z; 
						pointInScreen.y = sp.y / sp.z;

						if( pointInScreen.x > 0 && 
						    pointInScreen.x < w && 
							pointInScreen.y > 0 && 
							pointInScreen.y < h ){
							
							usedPoints.insert( feature->id() );
							features.push_back( feature );
							predictedPos.push_back( pointInScreen );
							descriptor.push_back( feature->descriptor() );
						}
					}
				}
			}
		}	
	}

	inline void StereoSLAM::estimateCameraPose( const PointSet3d & p3d, const PointSet2d & p2d )
	{
		Matrix3d K;
		const Matrix3f & kf = _camCalib0.intrinsics();
		K[ 0 ][ 0 ] = kf[ 0 ][ 0 ];	K[ 0 ][ 1 ] = kf[ 0 ][ 1 ]; K[ 0 ][ 2 ] = kf[ 0 ][ 2 ];
		K[ 1 ][ 0 ] = kf[ 1 ][ 0 ]; K[ 1 ][ 1 ] = kf[ 1 ][ 1 ]; K[ 1 ][ 2 ] = kf[ 1 ][ 2 ];
		K[ 2 ][ 0 ] = kf[ 2 ][ 0 ]; K[ 2 ][ 1 ] = kf[ 2 ][ 1 ]; K[ 2 ][ 2 ] = kf[ 2 ][ 2 ];

		/*	
		EPnPSAC sacModel( p3d, p2d, K );
		double maxReprojectionError = 6.0;
		double outlierProb = 0.1;
		RANSAC<EPnPSAC> ransac( sacModel, maxReprojectionError, outlierProb );

		size_t maxRansacIters = 200;
		Matrix4d m;
		m = ransac.estimate( maxRansacIters );
		*/
		
		EPnPd epnp( p3d );
		Matrix4d m;
		epnp.solve( m, p2d, K );

		Eigen::Matrix<double, 3, 3> Ke;
		Eigen::Matrix<double, 4, 4> extrC;
		Eigen::Matrix4d me;
		for( size_t i = 0; i < 3; i++ )
			for( size_t k = 0; k < 3; k++ )
				Ke( i, k ) = K[ i ][ k ];
		for( size_t i = 0; i < 4; i++ ){
			for( size_t k = 0; k < 4; k++ ){
				extrC( i, k ) = _camCalib0.extrinsics()[ i ][ k ];
				me( i, k ) = m[ i ][ k ];
			}
		}

		PointCorrespondences3d2d<double> pointCorresp( Ke, extrC );
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
		
		RobustHuber<double, PointCorrespondences3d2d<double>::MeasType> costFunction( 10.0 );
		LevenbergMarquard<double> lm;
		TerminationCriteria<double> termCriteria( TERM_COSTS_THRESH | TERM_MAX_ITER );
		termCriteria.setCostThreshold( 0.001 );
		termCriteria.setMaxIterations( 10 );
		lm.optimize( pointCorresp, costFunction, termCriteria );

		me = pointCorresp.pose().transformation().inverse();
		_pose.set( me );

		Matrix4f mf;
		for( size_t i = 0; i < 4; i++ )
			for( size_t k = 0; k < 4; k++ )
				mf[ i ][ k ] = me( i, k );
		newCameraPose.notify( mf );
	}
}

#endif
