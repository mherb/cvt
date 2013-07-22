#include "FlatSLAMMap.h"

using namespace std;


namespace cvt{

FlatSLAMMap::FlatSLAMMap(SlamMap map){

    int numberOfFeatures = map.numFeatures();
    int numberOfKeyframes = map.numKeyframes();

    for(int featureIndex = 0; featureIndex < numberOfFeatures; featureIndex++){
        double * currentEstimate = map.featureForId(featureIndex).estimate().data();
        for(int i = 0; i < 4; i++){
            features.push_back(currentEstimate[i]);
        }
    }

    for(int keyframeIndex = 0; keyframeIndex < numberOfKeyframes; keyframeIndex++){
        const double * pose = map.keyframeForId(keyframeIndex).pose().transformation().data();
        for(int i = 0; i < 16; i++){
            cameras.push_back(pose[i]);
        }
    }

    Eigen::Matrix3d mapIntrinsics = map.intrinsics();
    double* intrinsicsDouble = mapIntrinsics.data();
    for(unsigned int i = 0; i < sizeof(mapIntrinsics.data()); i++){
        intrinsics.push_back((float)intrinsicsDouble[i]);
    }

    Keyframe currentKeyframe;
    Keyframe::MeasurementIterator measEnd;
    Keyframe::MeasurementIterator measIt;
    int currentFeatureIndex;
    Eigen::Vector2d currentMeasurement;

    for(int keyframeIdx = 0; keyframeIdx < numberOfKeyframes; keyframeIdx++){
        currentKeyframe = map.keyframeForId(keyframeIdx);
        //Eigen::Matrix<double, 4, 4>

        measEnd = currentKeyframe.measurementsEnd();
        measIt = currentKeyframe.measurementsBegin();

        while(measIt != measEnd){

            currentFeatureIndex = measIt->first;

            featIdx.push_back(currentFeatureIndex);
            camIdx.push_back(keyframeIdx);

            currentMeasurement = measIt->second.point;
            measurements2D.push_back(currentMeasurement(0));
            measurements2D.push_back(currentMeasurement(1));

            measurementCounter++;
            measIt++;
        }
    }
}

FlatSLAMMap::~FlatSLAMMap(){

}


}//namespace cvt
