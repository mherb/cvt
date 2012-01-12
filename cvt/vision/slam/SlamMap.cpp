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
		_keyframes.push_back( Keyframe( pose, id ) );
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
										 const CameraCalibration& camCalib,
										 double maxDistance ) 
	{
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
						pointInCam = cameraPose * feature.estimate();
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

	void SlamMap::deserialize( XMLNode* node )
	{
		if( node->name() != "SlamMap" ){
			throw CVTException( "This is not a SlamMap node" );
		}

		// get intrinsics:
		Matrix3d K;
		K = Matrix3d::fromString( node->childByName( "Intrinsics" )->child( 0 )->value() );
		EigenBridge::toEigen( _intrinsics, K );

		XMLNode* keyframes = node->childByName( "Keyframes" );
		if( keyframes == NULL ){
			throw CVTException( "No Keyframes in MapFile!" );
		}

		size_t numKF = keyframes->childSize();
		_keyframes.resize( numKF );
		_numMeas = 0;
		for( size_t i = 0; i < _keyframes.size(); i++ ){
			XMLNode* kfNode = keyframes->child( i );

			// get the id: 			
			size_t kfId = kfNode->childByName( "id" )->value().toInteger();

			_keyframes[ kfId ].deserialize( kfNode );
			_numMeas += _keyframes[ kfId ].numMeasurements();
		}

		XMLNode* featureNodes = node->childByName( "MapFeatures" );
		if( featureNodes == NULL ){
			throw CVTException( "No Features in MapFile!" );
		}

		_features.resize( featureNodes->childSize() );
		for( size_t i = 0; i < _features.size(); i++ ){
			XMLNode* fNode = featureNodes->child( i );
			_features[ i ].deserialize( fNode );
		}
	}

	XMLNode* SlamMap::serialize() const
	{
		XMLElement* mapNode = new XMLElement( "SlamMap");

		// Intrinsics of the Keyframe images
		{
			Matrix3d K;
			EigenBridge::toCVT( K, _intrinsics );
			XMLElement* camIntrinsics = new XMLElement( "Intrinsics" );
			camIntrinsics->addChild( new XMLText( K.toString() ) );
			mapNode->addChild( camIntrinsics );
		}
		
		// the keyframes: serialize each one
		XMLElement* keyframeNodes = new XMLElement( "Keyframes" );
		for( size_t i = 0; i < _keyframes.size(); i++ ){
			keyframeNodes->addChild( _keyframes[ i ].serialize() );
		}
		mapNode->addChild( keyframeNodes );
		

		// the mapfeatures
		XMLElement* mapFeatureNodes = new XMLElement( "MapFeatures" );
		for( size_t i = 0; i < _features.size(); i++ ){
			mapFeatureNodes->addChild( _features[ i ].serialize() );
		}
		mapNode->addChild( mapFeatureNodes );

		return mapNode;
	}
}
