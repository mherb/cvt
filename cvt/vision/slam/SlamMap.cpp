#include <cvt/vision/slam/SlamMap.h>

#include <set>

namespace cvt
{
	SlamMap::SlamMap() :
		_numMeas( 0 )
	{
	}

	SlamMap::~SlamMap()
	{
	}

	size_t SlamMap::addKeyframe( const Eigen::Matrix4d& pose )
	{
		size_t id = _keyframes.size();
		_keyframes.push_back( Keyframe( pose ) );
		return id;
	}

			
	size_t SlamMap::addFeature( const MapFeature& world )
	{
		_features.push_back( world );
		return _features.size()-1;
	}

	size_t SlamMap::addFeatureToKeyframe( const MapFeature& world, 
									      const MapMeasurement& feature, 
										  size_t keyframeId )
	{
		size_t pointId = addFeature( world );
		addMeasurement( pointId, keyframeId, feature );	

		return pointId;
	}
			
	void SlamMap::addMeasurement( size_t pointId, 
								  size_t keyframeId,
								  const  MapMeasurement& meas )
	{
		_features[ pointId ].addPointTrack( keyframeId );
		_keyframes[ keyframeId ].addFeature( meas, pointId );
		_numMeas++;
	}


	size_t SlamMap::findClosestKeyframe( const Eigen::Matrix4d& worldT ) const
	{
		double nearest = 100000.0f;
		size_t kf = 0;

		for( size_t i = 0; i < _keyframes.size(); i++ ){
			double dist = _keyframes[ i ].distance( worldT );
			if( dist < nearest ){
				nearest = dist;
				kf = i;
			}
		}

		return kf;
	}
		
	void SlamMap::selectVisibleFeatures( std::vector<size_t> & visibleFeatureIds,
										 std::vector<Vector2f> & projections,
									     const Eigen::Matrix4d&	cameraPose,
										 const CameraCalibration& camCalib ) 
	{
		double maxDistance = 9.0;
		Eigen::Matrix4d poseInv;
		{
			Eigen::Matrix3d RInv = cameraPose.block<3, 3>( 0, 0 ).transpose();
			Eigen::Vector3d tInv = -RInv * cameraPose.block<3, 1>( 0, 3 );
			poseInv.block<3, 3>( 0, 0 ) = RInv;
			poseInv.block<3, 1>( 0, 3 ) = tInv;
			poseInv( 3, 0 ) = poseInv( 3, 1 ) = poseInv( 3, 2 ) = 0.0;
			poseInv( 3, 3 ) = 1.0;
		}

		Eigen::Vector4d pointInCam;
		Vector4f pic;
		Vector4f sp;
		Vector2f pointInScreen;

		Vector2f c = camCalib.center();
		c *= 2.0f;
		
		std::set<size_t> usedPoints;
		for( size_t i = 0; i < _keyframes.size(); i++ ){
			if( _keyframes[ i ].distance( cameraPose ) < maxDistance ){
				// check if the points of this keyframe would project to this camera
				const Keyframe& kf = _keyframes[ i ];
				
				Keyframe::MeasurementIterator iter = kf.measurementsBegin();
				const Keyframe::MeasurementIterator measEnd = kf.measurementsEnd();

				while( iter != measEnd ){
					size_t fId = iter->first;
					const MapFeature& feature = _features[ fId ];

					if( usedPoints.find( fId ) == usedPoints.end() ){
						pointInCam = poseInv * feature.estimate();
						pointInCam /= pointInCam[ 3 ];
						if( pointInCam[ 2 ] > 0.0 ){
							pic[ 0 ] = ( float )pointInCam[ 0 ];
							pic[ 1 ] = ( float )pointInCam[ 1 ];
							pic[ 2 ] = ( float )pointInCam[ 2 ];
							pic[ 3 ] = ( float )pointInCam[ 3 ];

							// project it to the screen:
							sp = camCalib.projectionMatrix() * pic;
							pointInScreen.x = sp.x / sp.z; 
							pointInScreen.y = sp.y / sp.z;

							if( pointInScreen.x > 0 && 
							    pointInScreen.x < c.x && 
							    pointInScreen.y > 0 && 
							    pointInScreen.y < c.y ){
								usedPoints.insert( fId );
								visibleFeatureIds.push_back( fId );
								projections.push_back( pointInScreen );
							}
						}
					}
					++iter;
				}
			}	
		}
	}
}
