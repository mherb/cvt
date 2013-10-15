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

            virtual void            next() = 0;

            virtual bool            hasGroundTruthPose() const { return false; }

            virtual cvt::Matrix4d   groundTruthPose() const
            {
                cvt::Matrix4d m;
                m.setIdentity();
                return m;
            }

			virtual const Image&	depth() const = 0;
			virtual const Image&	rgb()   const = 0;
			virtual double			stamp() const { return 0.0; }

            const CameraCalibration& calibration() const { return _calib; }
            void setCalibration( const CameraCalibration& calib ) { _calib = calib; }

        protected:
            CameraCalibration   _calib;

    };

}

#endif // CVT_RGBDINPUT_H
