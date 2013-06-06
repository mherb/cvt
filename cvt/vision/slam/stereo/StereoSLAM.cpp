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
   StereoSLAM::StereoSLAM( FeatureDetector* detector,
						   FeatureDescriptorExtractor* descExtractor,
						   const StereoCameraCalibration &calib ):
	   _detector( detector ),
	   _descExtractorLeft( descExtractor ),
	   _descExtractorRight( descExtractor->clone() ),
	   _pyrLeft( _params.pyramidOctaves, _params.pyramidScaleFactor ),
	   _pyrRight( _params.pyramidOctaves, _params.pyramidScaleFactor ),
	   _calib( calib ),
	   _activeKF( -1 )
   {
      Eigen::Matrix3d K;
	  EigenBridge::toEigen( K, calib.firstCamera().intrinsics() );
      _map.setIntrinsics( K );
   }

   void StereoSLAM::newImages( const Image& imgLeftGray, const Image& imgRightGray )
   {
       CVT_ASSERT( imgLeftGray.format() == IFormat::GRAY_UINT8, "INPUT IMAGES NEED TO BE GRAY_FLOAT" );
       CVT_ASSERT( imgRightGray.format() == IFormat::GRAY_UINT8, "INPUT IMAGES NEED TO BE GRAY_FLOAT" );

	  // detect current keypoints and extract descriptors
	  extractFeatures( imgLeftGray, imgRightGray );

	  std::cout << "CurrentFeatures Left: "  << _descExtractorLeft->size() << std::endl;
	  std::cout << "CurrentFeatures Right: " << _descExtractorRight->size() << std::endl;

	  // predict current visible features by projecting with current estimate of pose
	  std::vector<Vector2f> predictedPositions;
	  std::vector<size_t>   predictedFeatureIds;
	  std::vector<FeatureDescriptor*> predictedDescriptors;
	  // predict visible features based on last pose
	  Eigen::Matrix4d poseEigen = _pose.transformation().cast<double>();
	  predictVisibleFeatures( predictedPositions,
							  predictedFeatureIds,
							  predictedDescriptors,
							  poseEigen );

	  // match with current left features
	  std::vector<MatchingIndices> matchedIndices;
	  _descExtractorLeft->matchInWindow( matchedIndices,
										 predictedDescriptors,
										 _params.matchingWindow,
										 _params.matchingMaxDescDistance );

	  // build up the correspondences for camera tracking
	  PointSet2f points2d;
	  PointSet3f points3d;
	  std::vector<size_t> mapIndices;
	  createCorrespondences( points2d, points3d, mapIndices, matchedIndices, predictedFeatureIds );

	  size_t numTrackedFeatures = points3d.size();
	  numTrackedPoints.notify( numTrackedFeatures );

	  std::vector<size_t> trackingInliers;
	  if( numTrackedFeatures > 6 ){
		  estimateCameraPose( trackingInliers, points3d, points2d );
	  } else {
		  // too few features -> lost track: relocalization needed
		  std::cout << "Too few features tracked - relocalization needed" << std::endl;
	  }

	  if( newKeyframeNeeded( trackingInliers.size() ) ){
		  std::vector<const FeatureDescriptor*> freeFeaturesLeft;
		  sortOutFreeFeatures( freeFeaturesLeft, _descExtractorLeft, trackingInliers, matchedIndices );

		  // try to match the free features with right frame
		  std::vector<FeatureMatch> stereoMatches;
		  std::cout << "Free features: " << freeFeaturesLeft.size() << std::endl;
		  _descExtractorRight->scanLineMatch( stereoMatches,
											  freeFeaturesLeft,
											  _params.minDisparity,
											  _params.maxDisparity,
											  _params.matchingMaxDescDistance,
											  _params.maxEpilineDistance );		  

		  PointSet3f newPts3d;
		  std::vector<const FeatureDescriptor*> newDescriptors;
		  stereoMatchesTo3DPoints( newPts3d, newDescriptors, stereoMatches );

		  // wait until current ba thread is ready
		  if( _bundler.isRunning() ){
			  _bundler.join();
		  }


		  // a new keyframe should have a minimum number of features
		 if( ( newPts3d.size() + trackingInliers.size() ) > _params.minFeaturesForKeyframe ){
			 // create new keyframe with map features
			 addNewKeyframe( newDescriptors,
							 newPts3d,
							 points2d,
							 mapIndices,
							 trackingInliers );

			 /* bundle adjust if we have at least 2 keyframes*/
			 if( _map.numKeyframes() >  2 ){
			   //_bundler.run( &_map );
			 }

			 keyframeAdded.notify();
			 mapChanged.notify( _map );
			 std::cout << "New Keyframe Added" << std::endl;
			 std::cout << "Triangulated: " << newPts3d.size() << std::endl;

		 } else {
			 std::cout << "Could only triangulate " << newPts3d.size() << " new features " << std::endl;
		 }

	  }

	  Image debugMono;
	  imgLeftGray.convert( debugMono, IFormat::RGBA_UINT8 );
	  createDebugImageMono( debugMono, points2d, predictedPositions );
	  trackedFeatureImage.notify( debugMono );

      int last = _activeKF;
	  poseEigen = _pose.transformation().cast<double>();
	  _activeKF = _map.findClosestKeyframe( poseEigen );
      if( _activeKF != last )
         std::cout << "Active KF: " << _activeKF << std::endl;
   }

   void StereoSLAM::extractFeatures( const Image& left, const Image& right )
   {
	   // update image pyramid(s)
	   _pyrLeft.update( left );
	   _pyrRight.update( right );

	   // detect features in current left frame
	   FeatureSet leftFeatures, rightFeatures;
	   _detector->detect( leftFeatures, _pyrLeft );
	   _detector->detect( rightFeatures, _pyrRight );

	   leftFeatures.filterNMS( 1, true );
	   rightFeatures.filterNMS( 1, true );
	   leftFeatures.filterBest( 3000, true );
	   rightFeatures.filterBest( 3000, true );

	   // extract the descriptors
	   _descExtractorLeft->clear();
	   _descExtractorLeft->extract( _pyrLeft, leftFeatures );

	   _descExtractorRight->clear();
	   _descExtractorRight->extract( _pyrRight, rightFeatures );
   }

   void StereoSLAM::predictVisibleFeatures( std::vector<Vector2f>& imgPositions,
											std::vector<size_t>& ids,
											std::vector<FeatureDescriptor*>& descriptors,
											const Eigen::Matrix4d& cameraPose )
   {
	   _map.selectVisibleFeatures( ids,
								   imgPositions,
								   cameraPose,
								   _calib.firstCamera(),
								   _params.keyframeSelectionRadius );

	   std::cout << "Selected points: " << ids.size() << std::endl;

	   // get the corresponding descriptors
	   _descriptorDatabase.descriptorsForIds( descriptors, ids );
	   for( size_t i = 0; i < descriptors.size(); ++i ){
		   descriptors[ i ]->pt = imgPositions[ i ];
	   }
   }

   void StereoSLAM::createCorrespondences( PointSet2f& points2d,
										   PointSet3f& points3d, std::vector<size_t> &mapIndices,
										   const std::vector<MatchingIndices>& matchedIndices,
										   const std::vector<size_t>& ids ) const
   {
	   Vector4f vec;
	   mapIndices.reserve( matchedIndices.size() );
	   for( size_t i = 0; i < matchedIndices.size(); ++i ){
		   const MatchingIndices& m = matchedIndices[ i ];

		   size_t curMapIdx = ids[ m.srcIdx ];
		   mapIndices.push_back( curMapIdx );

		   const MapFeature& mapFeature = _map.featureForId( curMapIdx );
		   EigenBridge::toCVT( vec, mapFeature.estimate() );
		   points3d.add( Vector3f( vec ) );
		   points2d.add( (*_descExtractorLeft)[ m.dstIdx ].pt );
	   }
   }

   void StereoSLAM::estimateCameraPose( std::vector<size_t>& inlierIndices, const PointSet3f & p3d, const PointSet2f& p2d )
   {
	  const Matrix3f & k = _calib.firstCamera().intrinsics();

      //Matrix3f kinv( k.inverse() );
      //P3PSac model( p3d, p2d, k, kinv );
      //RANSAC<P3PSac> ransac( model, 5.0, 0.2 );

	  // TODO: integrate this with the pointset
	  // TODO: add Non.-Linear Pose estimation to point set

	  EPnPSAC<float> model( p3d, p2d, k );
	  RANSAC<EPnPSAC<float> > ransac( model, 5.0, 0.4 );

	  Matrix4f estimated;
	  estimated.setIdentity();
      float inlierPercentage = 0.0f;
//      float minInThresh = 0.6f;
//      size_t iter = 0;
//      while( inlierPercentage < minInThresh ){
//        estimated = ransac.estimate( 1000 );
//        inlierPercentage = (float)ransac.inlierIndices().size() / (float)p3d.size();
//        iter++;

//        if( iter == 10 ){
//            minInThresh *= 0.9f;
//            iter = 0;
//        }
//      }
      estimated = ransac.estimate( 1000 );
      inlierPercentage = (float)ransac.inlierIndices().size() / (float)p3d.size();
      std::cout << "Inlier Percentage: " << inlierPercentage << std::endl;

	  Eigen::Matrix4f me;
      EigenBridge::toEigen( me, estimated );
      _pose.set( me );

      inlierIndices = ransac.inlierIndices();

      Matrix4f mf;
      EigenBridge::toCVT( mf, me );
      newCameraPose.notify( mf );
   }

   void StereoSLAM::sortOutFreeFeatures( std::vector<const FeatureDescriptor*>& freeFeaturesLeft,
										 const FeatureDescriptorExtractor* extractor,
										 const std::vector<size_t>& inliers,
										 const std::vector<MatchingIndices>& matches ) const
   {
	   std::set<size_t> usedIds;
	   for( size_t i = 0; i < inliers.size(); ++i ){
		   const MatchingIndices& m = matches[ inliers[ i ] ];
		   usedIds.insert( m.dstIdx );
	   }

	   const std::set<size_t>::const_iterator setEnd = usedIds.end();
	   for( size_t i = 0; i < extractor->size(); ++i ){
		   if( usedIds.find( i ) == setEnd ){
			   const FeatureDescriptor& desc = ( *extractor )[ i ];
			   freeFeaturesLeft.push_back( &desc );
		   }
	   }
   }

   void StereoSLAM::stereoMatchesTo3DPoints( PointSet3f& newPts,
											 std::vector<const FeatureDescriptor*>& newDescriptors,
											 const std::vector<FeatureMatch>& stereoMatches ) const
   {
	   newPts.resize( stereoMatches.size() );
	   newDescriptors.resize( stereoMatches.size() );

	   float f = _calib.focalLength();
	   float b = _calib.baseLine();
	   float cx = _calib.firstCamera().intrinsics()[ 0 ][ 2 ];
	   float cy = _calib.firstCamera().intrinsics()[ 1 ][ 2 ];
	   float disp = 0.0f;
	   float bd = 0.0f;
	   for( size_t i = 0; i < stereoMatches.size(); ++i ){
		   const FeatureMatch& match = stereoMatches[ i ];
		   const FeatureDescriptor* left = ( const FeatureDescriptor* )match.feature0;
		   const FeatureDescriptor* right = ( const FeatureDescriptor* )match.feature1;

		   disp = left->pt[ 0 ] - right->pt[ 0 ];
		   bd = b / disp;

		   Vector3f& curPt = newPts[ i ];
		   curPt[ 0 ] = ( left->pt[ 0 ] - cx ) * bd;
		   curPt[ 1 ] = ( left->pt[ 1 ] - cy ) * bd;
		   curPt[ 2 ] = f * bd;

		   newDescriptors[ i ] = left;
	   }
   }

   void StereoSLAM::clear()
   {
      if( _bundler.isRunning() )
         _bundler.join();

      _map.clear();

	  Eigen::Matrix4f I( Eigen::Matrix4f::Identity() );
      _pose.set( I );
      _activeKF = -1;
   }

   void StereoSLAM::setPose( const Matrix4f& pose )
   {
	  _pose.set( pose );
   }


   void StereoSLAM::addNewKeyframe( const std::vector<const FeatureDescriptor*>& newDescriptors,
									const PointSet3f& newPoints3d,
									const PointSet2f& trackedMapPoints,
									const std::vector<size_t>& trackedMapIds,
									const std::vector<size_t>& inliers )
   {
	   // add a new Keyframe to the map
	   Eigen::Matrix4d transform( _pose.transformation().cast<double>() );
	   size_t kid = _map.addKeyframe( transform );

	   Eigen::Vector4d p3d;
	   MapMeasurement mm;
	   MapFeature     mf;
	   mm.information.setIdentity();
	   // add the tracked features to this keyframe
	   for( size_t i = 0; i < inliers.size(); i++ ){
		   // first get the inlier index
		   size_t idx = inliers[ i ];

		   // get the corresponding map idx
		   size_t mapIdx = trackedMapIds[ idx ];

		   EigenBridge::toEigen( mm.point, trackedMapPoints[ idx ] );
		   _map.addMeasurement( mapIdx, kid, mm );
	   }

	   // add the new 3d features to the map
	   Eigen::Matrix4d poseInv = transform.inverse();
	   for( size_t i = 0; i < newPoints3d.size(); ++i ){
		   const FeatureDescriptor* desc = newDescriptors[ i ];
		   const Vector2f& p2d = desc->pt;
		   const Vector3f& cP3d = newPoints3d[ i ];

		   EigenBridge::toEigen( mm.point, p2d );
		   p3d[ 0 ] = cP3d[ 0 ];
		   p3d[ 1 ] = cP3d[ 1 ];
		   p3d[ 2 ] = cP3d[ 2 ];
		   p3d[ 3 ] = 1;
           mf.estimate() = poseInv * p3d;
           size_t featureId = _map.addFeatureToKeyframe( mf, mm, kid );
		   _descriptorDatabase.addDescriptor( *desc, featureId );
       }
   }

   bool StereoSLAM::newKeyframeNeeded( size_t numTrackedFeatures ) const
   {
	  double kfDist = _params.maxKeyframeDistance + 1.0;
      if( _activeKF != -1 ){
		  Eigen::Matrix4d pe = _pose.transformation().cast<double>();
		  kfDist = _map.keyframeForId( _activeKF ).distance( pe );
      }

      // if distance is too far from active, always create a new one:
      if( kfDist > _params.maxKeyframeDistance ){
          std::cout << "New keyframe needed - too far from active keyframe: " << kfDist << std::endl;
          return true;
      }

      if( numTrackedFeatures < _params.minTrackedFeatures ){
          std::cout << "New keyframe needed - too few inliers: " << numTrackedFeatures << std::endl;
          return true;
      }

      return false;
   }

   void StereoSLAM::createDebugImageMono( Image & debugImage, const PointSet2f & tracked, const std::vector<Vector2f>& predPos ) const
   {
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


}
