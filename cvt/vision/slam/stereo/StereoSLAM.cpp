/*
         CVT - Computer Vision Tools Library

    Copyright (c) 2012, Philipp Heise, Sebastian Klose

   THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
   KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
   IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
   PARTICULAR PURPOSE.
 */
#include <cvt/vision/slam/stereo/StereoSLAM.h>


#include <cvt/math/sac/RANSAC.h>
#include <cvt/math/sac/P3PSac.h>
#include <cvt/math/sac/EPnPSAC.h>
#include <cvt/math/LevenbergMarquard.h>
#include <cvt/vision/PointCorrespondences3d2d.h>
#include <cvt/gfx/ifilter/IWarp.h>
#include <cvt/gfx/GFXEngineImage.h>
#include <cvt/vision/Vision.h>
#include <cvt/vision/slam/stereo/FeatureAnalyzer.h>
#include <cvt/util/Time.h>


namespace cvt
{
   StereoSLAM::StereoSLAM( FeatureTracking* ft, DepthInitializer* di ):
       _featureTracking( ft ),
       _depthInit( di ),
       _minTrackedFeatures( 50 ),
       _activeKF( -1 ),
       _minKeyframeDistance( 0.1 ),
       _maxKeyframeDistance( 0.4 )
   {
      Eigen::Matrix3d K;
      EigenBridge::toEigen( K, _depthInit->calibration0().intrinsics() );
      _map.setIntrinsics( K );
   }

   void StereoSLAM::newImages( const Image& img0, const Image& img1 )
   {
      // undistort the first image
      IWarp::apply( _undist0, img0, _depthInit->undistortionMap0() );

      // predict current visible features by projecting with current estimate of pose
      std::vector<Vector2f> predictedPositions;
      std::vector<size_t>   predictedFeatureIds;
      _map.selectVisibleFeatures(  predictedFeatureIds, predictedPositions,
                                  _pose.transformation(), _depthInit->calibration0(), 0.3f /* TODO: make it a param */  );

      // track the predicted features
      PointSet2d p2d;
      std::vector<size_t>   trackedIds;


      _featureTracking->trackFeatures( p2d, trackedIds,
                                       predictedPositions, predictedFeatureIds, _undist0 );

      Image debugMono;
      createDebugImageMono( debugMono, p2d, predictedPositions );
      trackedFeatureImage.notify( debugMono );

      /* get the 3d points from the map */
      PointSet3d p3d;
      fillPointsetFromIds( p3d, trackedIds );

      numTrackedPoints.notify( p2d.size() );
      size_t numTrackedFeatures = p3d.size();

      std::vector<size_t> trackingInliers;
      if( numTrackedFeatures > 6 ){
         estimateCameraPose( trackingInliers, p3d, p2d );
         std::cout << "Inliers: " << trackingInliers.size() << std::endl;
      } else {
         // too few features -> lost track: relocalization needed
          std::cout << "Too few features tracked - relocalization needed" << std::endl;
      }

      if( newKeyframeNeeded( trackingInliers.size() ) ){
         // undistort the second view
         IWarp::apply( _undist1, img1, _depthInit->undistortionMap1() );

         // wait until current ba thread is ready
         if( _bundler.isRunning() ){
            _bundler.join();
         }

         std::vector<Vector2f> avoidPos;
         avoidPos.resize( trackingInliers.size() );
         for( size_t i = 0; i < trackingInliers.size(); ++i ){
             size_t idx = trackingInliers[ i ];
             avoidPos[ i ].x = p2d[ idx ].x;
             avoidPos[ i ].y = p2d[ idx ].y;
         }

         std::vector<DepthInitializer::DepthInitResult> triangulated;
         _depthInit->triangulateFeatures( triangulated, avoidPos, _undist0, _undist1 );

         Image debugStereo;
         createDebugImageStereo( debugStereo, triangulated );
         newStereoView.notify( debugStereo );

         if( ( triangulated.size() + trackingInliers.size() ) > 10 ){
             // create new keyframe with map features
             addNewKeyframe( triangulated, p2d, trackedIds, trackingInliers );

             if( _map.numKeyframes() >  4 ){
               _bundler.run( &_map );
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

   void StereoSLAM::estimateCameraPose( std::vector<size_t>& inlierIndices, const PointSet3d & p3d, const PointSet2d & p2d )
   {
      const Matrix3f & kf = _depthInit->calibration0().intrinsics();
      Matrix3d k, kinv;
      for( size_t r = 0; r < 3; r++ )
          for( size_t c = 0; c < 3; c++ )
              k[ r ][ c ] = kf[ r ][ c ];
      kinv = k.inverse();

      //P3PSac model( p3d, p2d, k, kinv );
      //RANSAC<P3PSac> ransac( model, 5.0, 0.2 );
      EPnPSAC model( p3d, p2d, k );
      RANSAC<EPnPSAC> ransac( model, 5.0, 0.4 );


      Matrix4d estimated;
	  estimated.setIdentity();
      float inlierPercentage = 0.0f;

      float minInThresh = 0.6f;
      size_t iter = 0;
      while( inlierPercentage < minInThresh ){
        estimated = ransac.estimate( 3000 );
        inlierPercentage = (float)ransac.inlierIndices().size() / (float)p3d.size();
        std::cout << "Inlier Percentage: " << inlierPercentage << std::endl;
        iter++;

        if( iter == 10 ){
            minInThresh *= 0.9f;
            iter = 0;
        }
      }

      Eigen::Matrix4d me;
      EigenBridge::toEigen( me, estimated );
      _pose.set( me );

      inlierIndices = ransac.inlierIndices();

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
                                   const std::vector<size_t>& trackedIds,
                                   const std::vector<size_t>& inliers )
   {
       Eigen::Matrix4d poseInv = _pose.transformation().inverse();
       size_t kid = _map.addKeyframe( _pose.transformation() );

       Eigen::Vector4d p3d;
       MapMeasurement mm;
       MapFeature     mf;

       mm.information.setIdentity();
       mm.information *= 0.6;

       for( size_t i = 0; i < triangulated.size(); ++i ){
           const DepthInitializer::DepthInitResult & res = triangulated[ i ];
           EigenBridge::toEigen( mm.point, res.meas0 );
           EigenBridge::toEigen( p3d, res.point3d );

           mf.estimate() = poseInv * p3d;

           size_t featureId = _map.addFeatureToKeyframe( mf, mm, kid );
           _featureTracking->addFeatureToDatabase( res.meas0, featureId );

       }

       for( size_t i = 0; i < inliers.size(); i++ ){
           size_t idx = inliers[ i ];
           EigenBridge::toEigen( mm.point, p2d[ idx ] );
           _map.addMeasurement( trackedIds[ idx ], kid, mm );
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

      if( numTrackedFeatures < 10 )
          return true;

      // if too few features and minimum distance from last keyframe create new
      if( numTrackedFeatures < _minTrackedFeatures && kfDist > _minKeyframeDistance )
         return true;

      return false;
   }

   void StereoSLAM::createDebugImageMono( Image & debugImage, const PointSet2d & tracked, const std::vector<Vector2f>& predPos ) const
   {
       _undist0.convert( debugImage, IFormat::RGBA_UINT8 );

       GFXEngineImage ge( debugImage );
       GFX g( &ge );

       g.color() = Color::YELLOW;

       for( size_t i = 0; i < predPos.size(); i++ ){
           const Vector2f & p = predPos[ i ];
           g.fillRect( ( int )p.x - 2, ( int )p.y - 2, 5, 5 );
       }

       g.color() = Color::GREEN;

       for( size_t i = 0; i < tracked.size(); i++ ){
           const Vector2d & p = tracked[ i ];
           g.fillRect( ( int )p.x - 2, ( int )p.y - 2, 5, 5 );
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

   void StereoSLAM::fillPointsetFromIds( PointSet3d& pset,
                                         const std::vector<size_t>& trackedIds ) const
   {
      std::vector<size_t>::const_iterator it = trackedIds.begin();
      const std::vector<size_t>::const_iterator end = trackedIds.end();
      while( it != end ){
         const Eigen::Vector4d& p = _map.featureForId( *it ).estimate();
         double n = 1.0 / p[ 3 ];
         pset.add( Vector3d( p[ 0 ] * n, p[ 1 ] * n, p[ 2 ] * n  ) );
         it++;
      }
   }
}
