/*
            CVT - Computer Vision Tools Library

     Copyright (c) 2012, Philipp Heise, Sebastian Klose

    THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
    KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
    IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
    PARTICULAR PURPOSE.
 */

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
