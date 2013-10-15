
#ifndef CVT_STEREO_INPUT_H
#define CVT_STEREO_INPUT_H

#include <cvt/gfx/Image.h>
#include <cvt/vision/StereoCameraCalibration.h>

namespace cvt {
    class StereoInput {
        public:
            virtual ~StereoInput(){}

            virtual bool            nextFrame( size_t timeout ) = 0;

            /**
              left rectified image
             */
            virtual const Image&    left() const = 0;

            /**
              left rectified image
             */
            virtual const Image&    right() const = 0;

            /**
              calibration data (rectified rig)
             */
            virtual const StereoCameraCalibration& stereoCalib() const = 0;

    };
}

#endif
