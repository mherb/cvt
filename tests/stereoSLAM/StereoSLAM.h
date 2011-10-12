#ifndef CVT_STEREO_SLAM_H
#define CVT_STEREO_SLAM_H

#include <cvt/vision/CameraCalibration.h>
#include <cvt/gfx/ifilter/IWarp.h>
#include <cvt/vision/Vision.h>
#include <cvt/vision/ORB.h>
#include <cvt/vision/FeatureMatch.h>
#include <cvt/util/Signal.h>
#include "ORBStereoMatching.h"
#include "FeatureTracking.h"

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

			Signal<const ORBData*>	newORBData;	

		private:
			CameraCalibration	_camCalib0;
			CameraCalibration	_camCalib1;
			Image				_undistortMap0;
			Image				_undistortMap1;

			Image				_undist0;
			Image				_undist1;

			size_t				_orbOctaves;
			float				_orbScaleFactor;
			uint8_t				_orbCornerThreshold;
			size_t				_orbMaxFeatures;
			bool				_orbNonMaxSuppression;

			float				_matcherMaxLineDistance;	
			float				_matcherMaxDescriptorDist;
			ORBStereoMatching	_stereoMatcher;

			// FeatureTracker ...
			float				_trackingSearchRadius;
		    FeatureTracking		_featureTracking;	

			// triangulate from matches
			void triangulate( std::vector<Vector3f>& points, const std::vector<FeatureMatch>& matches ) const;
	};

	inline StereoSLAM::StereoSLAM( const CameraCalibration& c0,
								   size_t w0, size_t h0, 
								   const CameraCalibration& c1,
								   size_t w1, size_t h1 ):
		_camCalib0( c0 ), _camCalib1( c1 ),
		_matcherMaxLineDistance( 10.0f ),
		_matcherMaxDescriptorDist( 40 ),
		_stereoMatcher( _matcherMaxLineDistance, _matcherMaxDescriptorDist, _camCalib0, _camCalib1 ),
		_orbOctaves( 4 ), 
		_orbScaleFactor( 0.6f ),
		_orbCornerThreshold( 25 ),
		_orbMaxFeatures( 3000 ),
		_orbNonMaxSuppression( true ),
		_trackingSearchRadius( 30.0f ),
		_featureTracking( _matcherMaxDescriptorDist, _trackingSearchRadius )
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

		// match the features with predicted measurements from map
		// estimate the pose from 3d-2d measurements

		bool _newKeyframeNeeded = true;
		if( _newKeyframeNeeded ){
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

			// triangulate the 3d points
			std::vector<Vector3f> points3d;
			triangulate( points3d, matches );

			// add the new 3D points to the map!

			ORBData data;
			data.orb0 = &orb0;
			data.img0 = &_undist0;
			data.orb1 = &orb1;
			data.img1 = &_undist1;
			newORBData.notify( &data );
		}
	}


	inline void StereoSLAM::triangulate( std::vector<Vector3f>& points, const std::vector<FeatureMatch>& matches ) const
	{
		points.reserve( matches.size() );

		Vector4f tmp;
		Vector3f p3;
		for( size_t i = 0; i < matches.size(); i++ ){
			Vision::triangulate( tmp,
								_camCalib0.projectionMatrix(),
								_camCalib1.projectionMatrix(),
								matches[ i ].feature0->pt,
								matches[ i ].feature1->pt );
			p3 = tmp;

			if( p3.z > 0.0f )
				points.push_back( p3 );
		}
	}
}

#endif
