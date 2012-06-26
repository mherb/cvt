#ifndef STEREO_SLAM_APP_H
#define STEREO_SLAM_APP_H

#include <vector>
#include <cvt/io/VideoInput.h>
#include <cvt/gui/TimeoutHandler.h>
#include <cvt/util/Time.h>

#include <cvt/vision/slam/stereo/StereoSLAM.h>
#include <cvt/vision/slam/stereo/FeatureTracking.h>
#include "SLAMGui.h"

namespace cvt
{
   class StereoSLAMApp : public TimeoutHandler
   {
      public:
         StereoSLAMApp( const std::vector<VideoInput*> & cams,
                        const std::vector<CameraCalibration>& calibs );

         ~StereoSLAMApp();

         void onTimeout();

         void toggleStepping();

         void nextPressed() { _nextImage = true; }

         void saveMap();

         void clearMap();

         void saveImages();

      private:
         std::vector<VideoInput*>   _cams;
         FeatureTracking*           _featureTracking;
         DepthInitializer*          _depthInit;
         StereoSLAM                 _slam;
         Image                      _img0, _img1;

         uint32_t                   _timerId;

         SLAMGui                    _gui;

         Time                       _time;
         size_t                     _timeIter;

         bool                       _stepping;
         bool                       _nextImage;

   };


}

#endif
