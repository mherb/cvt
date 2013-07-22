#ifndef FLATSLAMMAP_H
#define FLATSLAMMAP_H

#include <cvt/vision/slam/SlamMap.h>


namespace cvt
{

    class FlatSLAMMap{

    public:
        FlatSLAMMap(SlamMap map);
        ~FlatSLAMMap();

        const float* getIntrinsics(){
            return intrinsics.data();
        }
        const float* getFeatures(){
            return features.data();
        }
        const float* getCameras(){
            return cameras.data();
        }
        const float* getMeasurements2D(){
            return measurements2D.data();
        }
        const float* getCamIdx(){
            return camIdx.data();
        }
        const float* getFeatIdx(){
            return featIdx.data();
        }

    private:
        std::vector<float> intrinsics;
        std::vector<float> features;
        std::vector<float> cameras;
        std::vector<float> measurements2D;
        std::vector<float> camIdx;
        std::vector<float> featIdx;
        long measurementCounter;

    };

}

#endif // FLATSLAMMAP_H
