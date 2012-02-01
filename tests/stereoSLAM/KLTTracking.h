#ifndef CVT_KLT_FEATURE_TRACKING_H
#define CVT_KLT_FEATURE_TRACKING_H

#include "FeatureTracking.h"

#include <cvt/vision/KLTTracker.h>

namespace cvt
{
	class KLTTracking : public FeatureTracking
	{
		public:
			KLTTracking( const CameraCalibration& c0, const CameraCalibration& c1 );


			void trackFeatures( PointSet3d&			p3d, 
								PointSet2d&			p2d,
								const SlamMap&		map,
								const SE3<double>&	pose,
								const Image&		img );

			bool checkFeature( const FeatureMatch& match, 
							   const Image& i0, 
							   const Image& i1 ) const;

			void clear();

			size_t triangulateNewFeatures( SlamMap& map, 
										   const SE3<double>& pose, 
										   const Image& first, 
										   const Image& second );

		private:
			typedef Translation2D<float> PoseType;

			const CameraCalibration&	_camCalib0;
			const CameraCalibration&	_camCalib1;
			ORBStereoMatching			_stereoMatcher;
			float						_maxTriangReprojError;

			std::vector<size_t>			_predictedIds;
			std::vector<Vector2f>		_predictedPositions;
			std::vector<size_t>			_trackedIndices;
			
			KLTTracker<PoseType>		_klt;
			std::vector<Vector2f>		_trackedPositions;

			typedef KLTPatch<16, PoseType> PatchType;
			std::vector<PatchType*>		_patchForId;
	};

	inline KLTTracking::KLTTracking( const CameraCalibration& c0, const CameraCalibration& c1 ) :
		_camCalib0( c0 ),
		_camCalib1( c1 ),
		_stereoMatcher( 5.0f/*maxLineDist*/, 60 /*maxDescdist*/, c0, c1 ),
		_maxTriangReprojError( 7.0f ),
		_klt( 10 )
	{
	}

	inline void KLTTracking::trackFeatures( PointSet3d&			p3d, 
										    PointSet2d&			p2d,
										    const SlamMap&		map,
										    const SE3<double>&	pose,
										    const Image&		img )
	{
		// predict ids and positions:
		_predictedIds.clear();
		_predictedPositions.clear();
		map.selectVisibleFeatures( _predictedIds, _predictedPositions, pose.transformation(), _camCalib0 );


		// initialize the KLTData:
		std::vector<Translation2D<float> > poses;
		std::vector<PatchType*>			   predPatches;

		poses.resize( _predictedIds.size() );
		predPatches.resize( _predictedIds.size() );
	    Vector2f offset;	
		for( size_t i = 0; i < _predictedIds.size(); i++ ){
			predPatches[ i ] = _patchForId[ _predictedIds[ i ] ];
			offset = predPatches[ i ]->position() - _predictedPositions[ i ];
			poses[ i ].set( offset.x, offset.y );
		}

		// track with klt:
		_trackedIndices.clear();
		_klt.trackFeatures( _trackedIndices, poses, predPatches, img );

		// create the correspondences:
		Vector3d p3;
		Vector2d p2;

		Eigen::Vector2f tmpa, tmpb;

		_trackedPositions.resize( _trackedIndices.size() );
		for( size_t i = 0; i < _trackedIndices.size(); i++ ){
			// got a match so add it to the point sets
			size_t currIdx = _trackedIndices[ i ];
			const MapFeature & mapFeat = map.featureForId( _predictedIds[ currIdx ] );
			p3.x = mapFeat.estimate().x(); 
			p3.y = mapFeat.estimate().y(); 
			p3.z = mapFeat.estimate().z();
			p3d.add( p3 );

			EigenBridge::toEigen( tmpa, predPatches[ currIdx ]->position() );
			
			poses[ currIdx ].transformInverse( tmpb, tmpa );
			EigenBridge::toCVT( _trackedPositions[ i ], tmpb );
			p2.x = _trackedPositions[ i ].x;
			p2.y = _trackedPositions[ i ].y;
			p2d.add( p2 );
		}
	}

	inline void KLTTracking::clear()
	{
		for( size_t i = 0; i < _patchForId.size(); i++ ){
			delete _patchForId[ i ];
		}
		_patchForId.clear();
	}

	inline size_t KLTTracking::triangulateNewFeatures( SlamMap& map, 
													   const SE3<double>& pose, 
													   const Image& first, 
													   const Image& second )
	{
		ORB orb0( first, 2, 0.5f, 20, 2000, true );
		ORB orb1( second, 2, 0.5f, 20, 2000, true );


		// find stereoMatches by avoiding already found matches
		std::set<size_t> doNotUse;
		for( size_t i = 0; i < orb0.size(); i++ ){
			const Vector2f& po = orb0[ i ].pt;
			for( size_t k = 0; k < _trackedPositions.size(); k++ ){
				if( ( po - _trackedPositions[ k ] ).lengthSqr() < 300.0f ){
					doNotUse.insert( i );
					break;
				}
			}
		}

		std::vector<FeatureMatch> stereoMatches;
		_stereoMatcher.matchEpipolar( stereoMatches, orb0, orb1, doNotUse );

		size_t numNew = 0;
		// Create a new keyframe with image 0 as reference image
		if( stereoMatches.size() > 0 ){

			// add a new keyframe to the map
			size_t kId = map.addKeyframe( pose.transformation() );

			Keyframe & keyframe = map.keyframeForId( kId );
			keyframe.setImage( first );

			MapMeasurement meas;
			// meas.information *= ( 1.0 / _trackingSearchRadius );

			Eigen::Matrix4d featureCov = Eigen::Matrix4d::Identity();
			MapFeature mapFeat( Eigen::Vector4d::Zero(), featureCov );

			// add the currently tracked features to the keyframe!
			for( size_t i = 0; i < _trackedIndices.size(); i++ ){
				size_t pointId = _predictedIds[ _trackedIndices[ i ] ];

				meas.point[ 0 ] = _trackedPositions[ i ].x;
				meas.point[ 1 ] = _trackedPositions[ i ].y;
				map.addMeasurement( pointId, kId, meas );
			}

			Vector2f p0, p1;
			Vector4f pNew;
			Eigen::Matrix4d poseInv = pose.transformation().inverse();
			size_t stride;
			const uint8_t* ptr = first.map( &stride );
			for( size_t i = 0; i < stereoMatches.size(); i++ ){
				const FeatureMatch& match = stereoMatches[ i ];
				if( match.feature1 ){
					if( checkFeatureSAD( match.feature0->pt, match.feature1->pt, first, second ) ){
						p0 = stereoMatches[ i ].feature0->pt;
						p1 = stereoMatches[ i ].feature1->pt;
						Vision::correctCorrespondencesSampson( p0, p1, _stereoMatcher.fundamental() );

						float reprErr = triangulateSinglePoint( pNew, p0, p1, _camCalib0.projectionMatrix(), _camCalib1.projectionMatrix() );
						if( reprErr < _maxTriangReprojError ){
							PatchType* newPatch = new PatchType();
							if( newPatch->update( ptr + ( ( int )p0.y * stride - 8 ) + ( int )p0.x - 8, stride ) ) {
								// good patch
								newPatch->position() = p0;
								meas.point[ 0 ] = p0.x;
								meas.point[ 1 ] = p0.y;

								Eigen::Vector4d & point = mapFeat.estimate();
								point[ 0 ] = pNew[ 0 ];
								point[ 1 ] = pNew[ 1 ];
								point[ 2 ] = pNew[ 2 ];
								point[ 3 ] = pNew[ 3 ];
								point = poseInv * point;

								size_t newPointId = map.addFeatureToKeyframe( mapFeat, meas, kId );
								if( newPointId != _patchForId.size() )
									std::cout << "IDS OUT OF SYNC!" << std::endl;

								_patchForId.push_back( newPatch );
								numNew++;
							} else {
								// bad patch: do not take
								delete newPatch;
							}
						}
					}
				}
			}
			first.unmap( ptr );
		}
		return numNew;
	}

	
}

#endif