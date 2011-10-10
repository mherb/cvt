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
				ORB* orb1;
			};

			StereoSLAM( const CameraCalibration& c0,
						size_t w0, size_t h0,
						const CameraCalibration& c1,
						size_t w1, size_t h1 );

			// new round with two new images, maybe also hand in a pose prediction?
			void newImages( const Image& img0, const Image& img1 );

			Signal<ORBData>	newORBData;	

		private:
			CameraCalibration	_camCalib0;
			CameraCalibration	_camCalib1;
			ORBStereoMatching	_stereoMatcher;
			Image				_undistortMap0;
			Image				_undistortMap1;

			size_t				_orbOctaves;
			float				_orbScaleFactor;
			uint8_t				_orbCornerThreshold;
			size_t				_orbMaxFeatures;
			bool				_orbNonMaxSuppression;

			float				_matcherMaxLineDistance;	
			float				_matcherMaxDescriptorDist;

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
		_stereoMatcher( _matcherMaxLineDistance, _matcherMaxDescriptorDist, _camCalib0, _camCalib1 ),
		_orbOctaves( 4 ), 
		_orbScaleFactor( 0.7f ),
		_orbCornerThreshold( 20 ),
		_orbMaxFeatures( 4000 ),
		_orbNonMaxSuppression( true ),
		_matcherMaxLineDistance( 30.0f ),
		_matcherMaxDescriptorDist( 200 ),
		_trackingSearchRadius( 25.0f ),
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
		Image tmp, gray0;
		
		// get grayscale image
		img0.convert( tmp, IFormat::GRAY_UINT8 );

		// undistort
		IWarp::apply( gray0, tmp, _undistortMap0 );

		// create the ORB
		ORB orb0( gray0, 
				  _orbOctaves, 
				  _orbScaleFactor,
				  _orbCornerThreshold,
				  _orbMaxFeatures,
				  _orbNonMaxSuppression );

		// match the features with predicted measurements from map

		bool _newKeyframeNeeded = true;
		if( _newKeyframeNeeded ){
			// get grayscale image
			img1.convert( tmp, IFormat::GRAY_UINT8 );

			// undistort
			Image gray1;
			IWarp::apply( gray1, tmp, _undistortMap1 );

			// create the ORB
			ORB orb1( gray1, 
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

			ORBData data;
			data.orb0 = &orb0;
			data.orb1 = &orb1;
			newORBData.notify( data );

			std::cout << "points3d: " << points3d.size() << std::endl;

		}
	}


	inline void StereoSLAM::triangulate( std::vector<Vector3f>& points, const std::vector<FeatureMatch>& matches ) const
	{
		points.resize( matches.size() );

		Vector4f tmp;
		for( size_t i = 0; i < matches.size(); i++ ){
			Vision::triangulate( tmp,
								_camCalib0.projectionMatrix(),
								_camCalib1.projectionMatrix(),
								matches[ i ].feature0->pt,
								matches[ i ].feature1->pt );
			points[ i ] = tmp;
		}
	}
}

#endif
