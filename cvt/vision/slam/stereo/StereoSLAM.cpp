/*
         CVT - Computer Vision Tools Library

    Copyright (c) 2012, Philipp Heise, Sebastian Klose

   THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
   KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
   IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
   PARTICULAR PURPOSE.
 */
#include <cvt/vision/slam/stereo/StereoSLAM.h>

//#include <cvt/vision/EPnP.h>
//#include <cvt/math/sac/RANSAC.h>
//#include <cvt/math/sac/EPnPSAC.h>
#include <cvt/math/LevenbergMarquard.h>
#include <cvt/vision/PointCorrespondences3d2d.h>
#include <cvt/gfx/ifilter/IWarp.h>
#include <cvt/gfx/GFXEngineImage.h>
#include <cvt/vision/Vision.h>
#include <cvt/vision/slam/stereo/FeatureAnalyzer.h>
#include <cvt/util/Time.h>


namespace cvt
{
   StereoSLAM::StereoSLAM( FeatureTracking* ft,
                           DepthInitializer* di,
                           size_t w0, size_t h0,
                           size_t w1, size_t h1 ):
       _featureTracking( ft ),
       _depthInit( di ),
       _minTrackedFeatures( 50 ),
       _activeKF( -1 ),
       _minKeyframeDistance( 0.05 ),
       _maxKeyframeDistance( 0.5 )
   {
       const CameraCalibration& c0 = _depthInit->calibration0();
       const CameraCalibration& c1 = _depthInit->calibration1();

      // create the undistortion maps
      _undistortMap0.reallocate( w0, h0, IFormat::GRAYALPHA_FLOAT );
      _undistortMap1.reallocate( w1, h1, IFormat::GRAYALPHA_FLOAT );
      createUndistortionMaps( c0, c1 );

      Eigen::Matrix3d K;
      EigenBridge::toEigen( K, c0.intrinsics() );
      _map.setIntrinsics( K );
   }

   void StereoSLAM::newImages( const Image& img0, const Image& img1 )
   {
      // undistort the first image
      IWarp::apply( _undist0, img0, _undistortMap0 );

      // predict current visible features by projecting with current estimate of pose
      std::vector<Vector2f> predictedPositions;
      std::vector<size_t>   predictedFeatureIds;
      _map.selectVisibleFeatures(  predictedFeatureIds, predictedPositions,
                                  _pose.transformation(), _depthInit->calibration0(), 3.0f /* TODO: make it a param */  );

      // track the predicted features
      PointSet2d p2d;
      std::vector<size_t>   trackedIds;
      _featureTracking->trackFeatures( p2d, trackedIds,
                                       predictedPositions, predictedFeatureIds, _undist0 );

      Image debugMono;
      createDebugImageMono( debugMono, p2d );
      trackedFeatureImage.notify( debugMono );

      /* get the 3d points from the map */
      PointSet3d p3d;
      fillPointsetFromIds( p3d, trackedIds );

      numTrackedPoints.notify( p2d.size() );
      size_t numTrackedFeatures = p3d.size();
      if( numTrackedFeatures > 6 ){
         estimateCameraPose( p3d, p2d );
      } else {
         // too few features -> lost track: relocalization needed
          std::cout << "Too few features tracked - relocalization needed" << std::endl;
      }

      if( newKeyframeNeeded( numTrackedFeatures ) ){
         // undistort the second view
         IWarp::apply( _undist1, img1, _undistortMap1 );

         // wait until current ba thread is ready
         if( _bundler.isRunning() ){
            _bundler.join();
         }

         std::vector<Vector2f> avoidPos;
         avoidPos.resize( p2d.size() );
         for( size_t i = 0; i < p2d.size(); ++i ){
             avoidPos[ i ].x = p2d[ i ].x;
             avoidPos[ i ].y = p2d[ i ].y;
         }

         std::vector<DepthInitializer::DepthInitResult> triangulated;
         _depthInit->triangulateFeatures( triangulated, avoidPos, _undist0, _undist1 );

         Image debugStereo;
         createDebugImageStereo( debugStereo, triangulated );
         newStereoView.notify( debugStereo );

         if( ( triangulated.size() + numTrackedFeatures ) > 10 ){
             // create new keyframe with map features
             addNewKeyframe( triangulated, p2d, trackedIds );

             if( _map.numKeyframes() > 1 ){
                //_bundler.run( &_map );
             }
             keyframeAdded.notify();
             mapChanged.notify( _map );
             std::cout << "New Keyframe Added" << std::endl;
             std::cout << "Triangulated: " << triangulated.size() << std::endl;

         } else {
             std::cout << "Could only triangulate " << triangulated.size() << " new features " << std::endl;
         }
      }

      int last = _activeKF;
      _activeKF = _map.findClosestKeyframe( _pose.transformation() );
      if( _activeKF != last )
         std::cout << "Active KF: " << _activeKF << std::endl;
   }

   void StereoSLAM::estimateCameraPose( const PointSet3d & p3d, const PointSet2d & p2d )
   {
      const Matrix3f & kf = _depthInit->calibration0().intrinsics();
      Eigen::Matrix<double, 3, 3> Ke;
      Eigen::Matrix<double, 4, 4> extrC;
      EigenBridge::toEigen( Ke, kf );
      EigenBridge::toEigen( extrC,  _depthInit->calibration0().extrinsics() );

      Eigen::Matrix4d me;
/*
      Matrix3d K;
      K[ 0 ][ 0 ] = kf[ 0 ][ 0 ]; K[ 0 ][ 1 ] = kf[ 0 ][ 1 ]; K[ 0 ][ 2 ] = kf[ 0 ][ 2 ];
      K[ 1 ][ 0 ] = kf[ 1 ][ 0 ]; K[ 1 ][ 1 ] = kf[ 1 ][ 1 ]; K[ 1 ][ 2 ] = kf[ 1 ][ 2 ];
      K[ 2 ][ 0 ] = kf[ 2 ][ 0 ]; K[ 2 ][ 1 ] = kf[ 2 ][ 1 ]; K[ 2 ][ 2 ] = kf[ 2 ][ 2 ];
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

      RobustHuber<double, PointCorrespondences3d2d<double>::MeasType> costFunction( 1.0 );
      LevenbergMarquard<double> lm;
      TerminationCriteria<double> termCriteria( TERM_COSTS_THRESH | TERM_MAX_ITER );
      termCriteria.setCostThreshold( 0.01 );
      termCriteria.setMaxIterations( 40 );
      lm.optimize( pointCorresp, costFunction, termCriteria );

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

   void StereoSLAM::addNewKeyframe( const std::vector<DepthInitializer::DepthInitResult> & triangulated,
                                   const PointSet2d& p2d,
                                   const std::vector<size_t>& trackedIds )
   {
       Eigen::Matrix4d poseInv = _pose.transformation().inverse();
       size_t kid = _map.addKeyframe( _pose.transformation() );

       Eigen::Vector4d p3d;
       MapMeasurement mm;
       MapFeature     mf;

       mm.information.setIdentity();

       for( size_t i = 0; i < triangulated.size(); ++i ){
           const DepthInitializer::DepthInitResult & res = triangulated[ i ];
           EigenBridge::toEigen( mm.point, res.meas0 );
           EigenBridge::toEigen( p3d, res.point3d );

           mf.estimate() = poseInv * p3d;

           size_t featureId = _map.addFeatureToKeyframe( mf, mm, kid );
           _featureTracking->addFeatureToDatabase( res.meas0, featureId );
       }

       for( size_t i = 0; i < trackedIds.size(); i++ ){
           EigenBridge::toEigen( mm.point, p2d[ i ] );
           _map.addMeasurement( trackedIds[ i ], kid, mm );
       }

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

   void StereoSLAM::createDebugImageMono( Image & debugImage, const PointSet2d & tracked ) const
   {
       _undist0.convert( debugImage, IFormat::RGBA_UINT8 );

       GFXEngineImage ge( debugImage );
       GFX g( &ge );

       g.color() = Color::GREEN;

       for( size_t i = 0; i < tracked.size(); i++ ){
           const Vector2d & p = tracked[ i ];
           g.fillRect( ( int )p.x - 1, ( int )p.y - 1, 3, 3 );
       }
   }

   void StereoSLAM::createDebugImageStereo( Image & debugImage,
                                            const std::vector<DepthInitializer::DepthInitResult>& triang ) const
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
         for( size_t i = 0; i < triang.size(); i++ ){
            const DepthInitializer::DepthInitResult & res = triang[ i ];
            Vector2f p2 = res.meas1;
            p2.x += _undist0.width();
            g.drawLine( res.meas0, p2 );
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

   void StereoSLAM::createUndistortionMaps( const CameraCalibration& c0, const CameraCalibration& c1 )
   {
       size_t w0 = _undistortMap0.width();
       size_t h0 = _undistortMap0.height();
       size_t w1 = _undistortMap1.width();
       size_t h1 = _undistortMap1.height();

       Vector3f radial = c0.radialDistortion();
       Vector2f tangential = c0.tangentialDistortion();
       float fx = c0.intrinsics()[ 0 ][ 0 ];
       float fy = c0.intrinsics()[ 1 ][ 1 ];
       float cx = c0.intrinsics()[ 0 ][ 2 ];
       float cy = c0.intrinsics()[ 1 ][ 2 ];
       IWarp::warpUndistort( _undistortMap0, radial[ 0 ], radial[ 1 ], cx, cy, fx, fy, w0, h0, radial[ 2 ], tangential[ 0 ], tangential[ 1 ] );

       radial = c1.radialDistortion();
       tangential = c1.tangentialDistortion();
       fx = c1.intrinsics()[ 0 ][ 0 ];
       fy = c1.intrinsics()[ 1 ][ 1 ];
       cx = c1.intrinsics()[ 0 ][ 2 ];
       cy = c1.intrinsics()[ 1 ][ 2 ];
       IWarp::warpUndistort( _undistortMap1, radial[ 0 ], radial[ 1 ], cx, cy, fx, fy, w1, h1, radial[ 2 ], tangential[ 0 ], tangential[ 1 ] );
   }
}
