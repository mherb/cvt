/*
   The MIT License (MIT)

   Copyright (c) 2011 - 2013, Philipp Heise and Sebastian Klose

   Permission is hereby granted, free of charge, to any person obtaining a copy
   of this software and associated documentation files (the "Software"), to deal
   in the Software without restriction, including without limitation the rights
   to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
   copies of the Software, and to permit persons to whom the Software is
   furnished to do so, subject to the following conditions:

   The above copyright notice and this permission notice shall be included in
   all copies or substantial portions of the Software.

   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
   OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
   THE SOFTWARE.
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
#include <cvt/vision/features/RowLookupTable.h>
#include <cvt/vision/slam/stereo/FeatureAnalyzer.h>
#include <cvt/util/Time.h>

namespace cvt
{
    StereoSLAM::StereoSLAM( FeatureDetector* detector,
                            FeatureDescriptorExtractor* descExtractor,
                            const StereoCameraCalibration &calib ,
                            const Params &params ):
        _detector( detector ),
        _descExtractorLeft( descExtractor ),
        _descExtractorRight( descExtractor->clone() ),
        _pyrLeft( _params.pyramidOctaves, _params.pyramidScaleFactor ),
        _pyrRight( _params.pyramidOctaves, _params.pyramidScaleFactor ),
        _pyrLeftf( _params.pyramidOctaves, _params.pyramidScaleFactor ),
        _pyrRightf( _params.pyramidOctaves, _params.pyramidScaleFactor ),
        _gradXl( _params.pyramidOctaves, _params.pyramidScaleFactor ),
        _gradYl( _params.pyramidOctaves, _params.pyramidScaleFactor ),
        _kernelGx( IKernel::HAAR_HORIZONTAL_3 ),
        _kernelGy( IKernel::HAAR_VERTICAL_3 ),
        _calib( calib ),
        _activeKF( -1 ),
        _params( params )
    {
        _kernelGx.scale( -0.5f );
        _kernelGy.scale( -0.5f );

        Eigen::Matrix3d K;
        EigenBridge::toEigen( K, calib.firstCamera().intrinsics() );
        _map.setIntrinsics( K );
    }

   void StereoSLAM::newImages( const Image& imgLeftGray, const Image& imgRightGray )
   {
	   CVT_ASSERT( imgLeftGray.format() == IFormat::GRAY_UINT8, "INPUT IMAGES NEED TO BE GRAY_UINT8" );
	   CVT_ASSERT( imgRightGray.format() == IFormat::GRAY_UINT8, "INPUT IMAGES NEED TO BE GRAY_UINT8" );

	  // detect current keypoints and extract descriptors
	  extractFeatures( imgLeftGray, imgRightGray );


	  std::cout << "CurrentFeatures Left: "  << _descExtractorLeft->size() << std::endl;
	  std::cout << "CurrentFeatures Right: " << _descExtractorRight->size() << std::endl;

	  // predict current visible features by projecting with current estimate of pose
	  std::vector<Vector2f> predictedPositions;
	  std::vector<size_t>   predictedFeatureIds;
	  std::vector<FeatureDescriptor*> predictedDescriptors;
	  std::vector<PatchType*> predictedPatches;

	  // predict visible features based on last pose
	  Eigen::Matrix4d poseEigen = _pose.transformation().cast<double>();
	  predictVisibleFeatures( predictedPositions,
							  predictedFeatureIds,
							  predictedDescriptors,
							  predictedPatches,
							  poseEigen );

	  TrackedFeatures tracked;
	  std::vector<MatchingIndices> matchedIndices;
	  trackPredictedFeatures( tracked,
							  matchedIndices,
							  predictedDescriptors,
							  predictedPatches,
							  predictedFeatureIds );

	  size_t numTrackedFeatures = tracked.size();
	  numTrackedPoints.notify( numTrackedFeatures );

	  std::vector<size_t> trackingInliers;
	  estimateCameraPose( trackingInliers, tracked.points3d, tracked.points2d );

	  if( newKeyframeNeeded( trackingInliers.size() ) ){
		  PointSet3f newPts3d;
		  std::vector<const FeatureDescriptor*> newDescriptors;
		  std::vector<PatchType*> newPatches;
		  initNewStereoFeatures( newPts3d, newDescriptors, newPatches, trackingInliers, matchedIndices );

		  // create new keyframe with map features
		  addNewKeyframe( newDescriptors,
						  newPatches,
						  newPts3d,
						  tracked.points2d,
						  tracked.mapFeatureIds,
						  trackingInliers );
	  }

	  Image debugMono;
	  imgLeftGray.convert( debugMono, IFormat::RGBA_UINT8 );
	  // TODO: only draw inliers of pose estimation
	  createDebugImageMono( debugMono, tracked.points2d, predictedPositions );
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

	   leftFeatures.filterNMS( 5, true );
	   rightFeatures.filterNMS( 5, true );

	   // TODO: gridfiltering instead of best N
	   leftFeatures.filterBest( 3000, true );
	   leftFeatures.sortPosition();

	   // maybe do not filter the right features?
	   rightFeatures.filterBest( 3000, true );
	   rightFeatures.sortPosition();

	   // extract the descriptors
	   _descExtractorLeft->clear();
	   _descExtractorLeft->extract( _pyrLeft, leftFeatures );

	   _descExtractorRight->clear();
	   _descExtractorRight->extract( _pyrRight, rightFeatures );
   }

   void StereoSLAM::predictVisibleFeatures( std::vector<Vector2f>& imgPositions,
											std::vector<size_t>& ids,
											std::vector<FeatureDescriptor*>& descriptors,
											std::vector<PatchType*>& patches,
											const Eigen::Matrix4d& cameraPose )
   {
	   _map.selectVisibleFeatures( ids,
								   imgPositions,
								   cameraPose,
								   _calib.firstCamera(),
								   _params.keyframeSelectionRadius );

	   std::cout << "Selected points: " << ids.size() << std::endl;

	   // get the corresponding descriptors
	   _descriptorDatabase.descriptorsAndPatchesForIds( descriptors, patches, ids );
	   for( size_t i = 0; i < descriptors.size(); ++i ){
		   descriptors[ i ]->pt = imgPositions[ i ];
	   }
   }

   void StereoSLAM::trackPredictedFeatures( TrackedFeatures& tracked,
											std::vector<MatchingIndices>& matchedIndices,
											const std::vector<FeatureDescriptor*>& predictedDescriptors,
											std::vector<StereoSLAM::PatchType*>& predictedPatches,
											const std::vector<size_t>& predictedIds )
   {
	   _pyrLeft.convert( _pyrLeftf, IFormat::GRAY_FLOAT  );


	   // match with current left features
	   RowLookupTable rlt( *_descExtractorLeft );
	   _descExtractorLeft->matchInWindow( matchedIndices,
										  rlt,
										  predictedDescriptors,
										  _params.matchingWindow,
										  _params.matchingMaxDescDistance );

		 // refine matched positions using KLT
		 tracked.reserve( matchedIndices.size() );
		 Vector4f vec;
		 Vector2f refined;
		 for( size_t i = 0; i < matchedIndices.size(); ++i ){
			 const MatchingIndices& m = matchedIndices[ i ];
			 size_t curMapIdx = predictedIds[ m.srcIdx ];
			 PatchType* patch = predictedPatches[ m.srcIdx ];

			 // refine the position
			 const Vector2f& pt = ( *_descExtractorLeft )[ m.dstIdx ].pt;
			 // TODO: try to only update the position and keep the rest of the patch pose
			 patch->initPose( pt );
			 patch->align( _pyrLeftf, 1 /*per octave!*/ );
			 patch->currentCenter( refined );

			 const MapFeature& mapFeature = _map.featureForId( curMapIdx );
			 EigenBridge::toCVT( vec, mapFeature.estimate() );

			 tracked.points3d.add( Vector3f( vec ) );
			 tracked.points2d.add( refined );
			 tracked.mapFeatureIds.push_back( curMapIdx );
		 }
   }

   void StereoSLAM::estimateCameraPose( std::vector<size_t>& inlierIndices, const PointSet3f & p3d, const PointSet2f& p2d )
   {
	   if( p3d.size() < 6 ){
		   // too few features -> lost track: relocalization needed
		   std::cout << "Too few features tracked - relocalization needed" << std::endl;
		   return;
	   }

	   const Matrix3f & k = _calib.firstCamera().intrinsics();

//	   Matrix3f kinv( k.inverse() );
//	   P3PSac<float> model( p3d, p2d, k, kinv );
//	   RANSAC<P3PSac<float> > ransac( model, 5.0, 0.5 );

	   EPnPSAC<float> model( p3d, p2d, k );
	   RANSAC<EPnPSAC<float> > ransac( model, 5.0, 0.5 );

	   Matrix4f estimated;
	   estimated.setIdentity();
	   estimated = ransac.estimate( 2000 );
	   float inlierPercentage = (float)ransac.inlierIndices().size() / (float)p3d.size();
	   std::cout << "Inlier Percentage: " << inlierPercentage << std::endl;

	   Eigen::Matrix4f me;
	   EigenBridge::toEigen( me, estimated );
	   _pose.set( me );

	   inlierIndices = ransac.inlierIndices();

	   newCameraPose.notify( estimated );
   }

   void StereoSLAM::initNewStereoFeatures( PointSet3f& newPts3d,
										   std::vector<const FeatureDescriptor*>& newDescriptors,
										   std::vector<PatchType*>& newPatches,
										   const std::vector<size_t>& trackingInliers,
										   const std::vector<MatchingIndices>& matchedIndices )
   {
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

	   // left is already converted to float
	   _pyrLeftf.convolve( _gradXl, _kernelGx );
	   _pyrLeftf.convolve( _gradYl, _kernelGy );
	   _pyrRight.convert( _pyrRightf, IFormat::GRAY_FLOAT );
	   // maybe also update the patches of the currently tracked features

	   // subpixel refinement of the stereo matches
	   newPts3d.reserve( stereoMatches.size() );
	   newDescriptors.reserve( stereoMatches.size() );
	   newPatches.reserve( stereoMatches.size() );
	   Vector2f rnew;

	   float f = _calib.focalLength();
	   float b = _calib.baseLine();
	   float cx = _calib.firstCamera().intrinsics()[ 0 ][ 2 ];
	   float cy = _calib.firstCamera().intrinsics()[ 1 ][ 2 ];
	   float disp = 0.0f;
	   float bd = 0.0f;
	   for( size_t i = 0; i < stereoMatches.size(); ++i ){
		   DescriptorDatabase::PatchType* patch = new DescriptorDatabase::PatchType( _pyrLeft.octaves() );
		   FeatureMatch& m = stereoMatches[ i ];
		   const Vector2f& posL = m.feature0->pt;
		   const Vector2f& posR = m.feature1->pt;
		   patch->update( _pyrLeftf, _gradXl, _gradYl, posL );
		   patch->initPose( posR );
		   patch->align( _pyrRightf, _params.kltStereoIters );
		   patch->currentCenter( rnew );

		   float dy = Math::abs( posL[ 1 ] - rnew[ 1 ] );
		   if( dy < _params.maxEpilineDistance ){
			   // init the 3d point
			   disp = posL[ 0 ] - rnew[ 0 ];
			   bd = b / disp;

			   newPts3d.add( Vector3f( ( posL[ 0 ] - cx ) * bd,
									   ( posL[ 1 ] - cy ) * bd,
									   f * bd ) );

			   newDescriptors.push_back( ( const FeatureDescriptor* )m.feature0 );
			   newPatches.push_back( patch );
           } else {
               delete patch;
           }
	   }
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
									std::vector<PatchType*>& newPatches,
									const PointSet3f& newPoints3d,
									const PointSet2f& trackedMapPoints,
									const std::vector<size_t>& trackedMapIds,
									const std::vector<size_t>& inliers )
   {
	   // a new keyframe should have a minimum number of features
	  if( ( newPoints3d.size() + trackedMapPoints.size() ) < _params.minFeaturesForKeyframe ){
		  std::cout << "Could only triangulate " << newPoints3d.size() << " new features " << std::endl;
		  return;
	  }
	  // wait until current ba thread is ready
	  if( _bundler.isRunning() ){
		  _bundler.join();
	  }

	  keyframeAdded.notify();
	  mapChanged.notify( _map );
	  std::cout << "Triangulated: " << newPoints3d.size() << std::endl;
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
		   PatchType* patch = newPatches[ i ];
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
		   _descriptorDatabase.addPatch( patch, featureId );
       }

	   /* bundle adjust if we have at least 2 keyframes*/
	   if( _map.numKeyframes() >  2 ){
		 //_bundler.run( &_map );
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
