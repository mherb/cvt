#include "FlatSLAMMap.h"

using namespace std;

namespace cvt{

    FlatSLAMMap::FlatSLAMMap( const SlamMap& map ){

        //Load features
        int numberOfFeatures = map.numFeatures();
        for(int featureIndex = 0; featureIndex < numberOfFeatures; featureIndex++){
            Vector4f currentEstimate;
            EigenBridge::toCVT(currentEstimate, map.featureForId(featureIndex).estimate());
            _features.push_back(currentEstimate);
        }

        //Load keyframes
        int numberOfKeyframes = map.numKeyframes();
        for(int keyframeIndex = 0; keyframeIndex < numberOfKeyframes; keyframeIndex++){
            Matrix4f pose;
            EigenBridge::toCVT(pose, map.keyframeForId(keyframeIndex).pose().transformation());
            _cameras.push_back(pose);
        }

        //Load intrinsics
        cvt::Matrix3f mapIntrinsics;
        EigenBridge::toCVT(mapIntrinsics, map.intrinsics());
        _intrinsics.push_back(mapIntrinsics);


        //Load Measurements and fill the camera and the feature index arrays
        Keyframe currentKeyframe;
        Keyframe::MeasurementIterator measEnd;
        Keyframe::MeasurementIterator measIt;
        int currentFeatureIndex;
        Vector2f currentMeasurement;

        for(int keyframeIdx = 0; keyframeIdx < numberOfKeyframes; keyframeIdx++){
            currentKeyframe = map.keyframeForId(keyframeIdx);
            //Eigen::Matrix<double, 4, 4>

            measEnd = currentKeyframe.measurementsEnd();
            measIt = currentKeyframe.measurementsBegin();

            while(measIt != measEnd){

                currentFeatureIndex = measIt->first;

                _featIdx.push_back(currentFeatureIndex);
                _camIdx.push_back(keyframeIdx);

                EigenBridge::toCVT(currentMeasurement, measIt->second.point );
                _measurements2D.push_back(currentMeasurement);

                _measurementCounter++;
                measIt++;
            }
        }

    }

    FlatSLAMMap::~FlatSLAMMap(){

    }


}//namespace cvt
