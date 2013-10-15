#ifndef CVT_ORB_STEREO_INIT_H
#define CVT_ORB_STEREO_INIT_H

#include <cvt/vision/slam/stereo/DepthInitializer.h>
#include <cvt/vision/slam/stereo/ORBStereoMatching.h>

namespace cvt
{
   class ORBStereoInit : public DepthInitializer
   {
      public:
          ORBStereoInit( const CameraCalibration& c0, const CameraCalibration& c1, size_t w, size_t h );

          void triangulateFeatures( std::vector<DepthInitResult> & triangulated,
                                    const std::vector<Vector2f> & avoidPositionsImg0,
                                    const Image& view0, const Image& view1 );

          ParamSet& parameters() { return _pset; }

          struct Parameters {
              float     maxEpilineDistance;
              float     maxDescriptorDistance;
              float     maxReprojectionError;
              float     minDepth;
              float     maxDepth;
              uint8_t   fastThreshold;
              uint32_t  orbMaxFeatures;
          };

      private:
          ORBStereoMatching     _matcher;
          ParamSet              _pset;
   };

}

#endif
