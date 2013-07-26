#include "FlatSLAMMap.h"

using namespace std;

namespace cvt
{

	FlatSLAMMap::FlatSLAMMap( const SlamMap& map )
	{

		_measurementCounter = 0;

		//Load features
		uint32_t numberOfFeatures = ( uint32_t ) map.numFeatures( );
		for( uint32_t featureIndex = 0; featureIndex < numberOfFeatures; featureIndex++ ) {
			Vector4f currentEstimate;
			EigenBridge::toCVT( currentEstimate, map.featureForId( featureIndex ).estimate( ) );
			_features.push_back( currentEstimate );
		}

		//Load keyframes
		uint32_t numberOfKeyframes = ( uint32_t ) map.numKeyframes( );
		for( uint32_t keyframeIndex = 0; keyframeIndex < numberOfKeyframes; keyframeIndex++ ) {
			Matrix4f pose;
			EigenBridge::toCVT( pose, map.keyframeForId( keyframeIndex ).pose( ).transformation( ) );
			_cameras.push_back( pose );
		}

		//Load intrinsics
		cvt::Matrix3f mapIntrinsics;
		EigenBridge::toCVT( mapIntrinsics, map.intrinsics( ) );
		_intrinsics.push_back( mapIntrinsics );


		//Load Measurements and fill the camera and the feature index arrays
		Keyframe currentKeyframe;
		Keyframe::MeasurementIterator measEnd;
		Keyframe::MeasurementIterator measIt;
		uint32_t currentFeatureIndex;
		Vector2f currentMeasurement;

		for( uint32_t keyframeIdx = 0; keyframeIdx < numberOfKeyframes; keyframeIdx++ ) {
			currentKeyframe = map.keyframeForId( keyframeIdx );
			//Eigen::Matrix<double, 4, 4>

			measEnd = currentKeyframe.measurementsEnd( );
			measIt = currentKeyframe.measurementsBegin( );

			while( measIt != measEnd ) {

				currentFeatureIndex = ( uint32_t ) measIt->first;

				_featIdx.push_back( ( size_t ) currentFeatureIndex );
				_camIdx.push_back( ( size_t ) keyframeIdx );

				EigenBridge::toCVT( currentMeasurement, measIt->second.point );
				_measurements2D.push_back( currentMeasurement );

				_measurementCounter++;
				measIt++;
			}
		}

	}

	FlatSLAMMap::~FlatSLAMMap( ){}

} //namespace cvt
