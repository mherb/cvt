/*
            CVT - Computer Vision Tools Library

     Copyright (c) 2012, Philipp Heise, Sebastian Klose

    THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
    KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
    IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
    PARTICULAR PURPOSE.
 */
#ifndef CVT_RGBDINPUT_H
#define CVT_RGBDINPUT_H

#include <cvt/gfx/Image.h>
#include <cvt/math/Matrix.h>
#include <cvt/vision/CameraCalibration.h>

namespace cvt {

    class RGBDInput
    {
        public:
            virtual ~RGBDInput(){}

            virtual void            nextFrame() = 0;

            virtual bool            hasGroundTruthPose() const = 0;
            virtual cvt::Matrix4d   groundTruthPose() const = 0;

            virtual const Image&             depth() const = 0;
            virtual const Image&             rgb() const = 0;

            const CameraCalibration& calibration() const { return _calib; }
            void setCalibration( const CameraCalibration& calib ) { _calib = calib; }

        protected:
            CameraCalibration   _calib;

    };

}

#endif // CVT_RGBDINPUT_H
