/*
            CVT - Computer Vision Tools Library

     Copyright (c) 2012, Philipp Heise, Sebastian Klose

    THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
    KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
    IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
    PARTICULAR PURPOSE.
 */
#ifndef CVT_CHAMELEONSTEREO_H
#define CVT_CHAMELEONSTEREO_H

#include <cvt/gfx/Image.h>
#include <cvt/io/StereoInput.h>
#include <cvt/io/DC1394Camera.h>
#include <cvt/vision/StereoCameraCalibration.h>

namespace cvt {

    class ChameleonStereo : public StereoInput
    {
        public:

            struct Parameters {
                cvt::String leftId;
                cvt::String rightId;
                int leftStrobePin;
                int rightTriggerPin;
            };

            ChameleonStereo( const Parameters& params );

            ~ChameleonStereo();

            bool nextFrame( size_t timeout );

            /**
              left rectified image
             */
            const Image& left() const { return _leftRect; }

            /**
              left rectified image
             */
            const Image& right() const { return _rightRect; }

            /**
              calibration data (rectified rig)
             */
            const StereoCameraCalibration& stereoCalib() const { return _calib; }

            const String& leftId()  const { return _leftCam->identifier(); }
            const String& rightId() const { return _rightCam->identifier(); }

            void trigger() const { _leftCam->triggerFrame(); }

            /**
             *  @brief set shutter value
             *  @param val  - shutter value in seconds
             */
            void setShutter( float val );

            /**
             *  @brief set gain value
             *  @param val  - gain value in db
             */
            void setGain( float val );

            void setAutoShutter( bool val );
            void setAutoGain( bool val );
            void setAutoExposure( bool val );
            void setExposure( float val );

            void enableAutoExposure( bool val );

        private:
            StereoCameraCalibration _calib;

            DC1394Camera*           _leftCam;
            DC1394Camera*           _rightCam;

            Image                   _leftRect;
            Image                   _rightRect;

            size_t idxForId( const cvt::String& id ) const;

            void configureLeft( int strobePin );
            void configureRight( int triggerPin );
            void configureStrobe( int pin );

    };

}

#endif // CVT_CHAMELEONSTEREO_H
