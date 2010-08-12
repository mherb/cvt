#ifndef TOOLS_ABSTRACT_CAMERA
#define TOOLS_ABSTRACT_CAMERA

#include <opencv/cv.h>

namespace cvt {
    struct CameraParameters {
        CameraParameters():
            width(752),
            height(480),
            horizonticalMirror(false),
            verticalMirror(false),
            autoWhiteBalance(false),
            autoGain(false),
            autoShutter(false),
            autoSensorShutter(false),
            fps(60.0),
            pixelClock(20.0),
            maxAutoShutter(33.0),
            exposureTime(33.0)
        {
        }

        unsigned int width;
        unsigned int height;

        bool horizonticalMirror;
        bool verticalMirror;
        bool autoWhiteBalance;
        bool autoGain;
        bool autoShutter;
        bool autoSensorShutter;

        double fps;
        unsigned int pixelClock;
        double maxAutoShutter;
        double exposureTime;
    };

    class AbstractCamera
    {
        public:
            AbstractCamera(CameraParameters const& params = CameraParameters()):
                _width(params.width),
                _height(params.height)
        {
        }

            virtual ~AbstractCamera(){};

            virtual void init() = 0;

            virtual void captureNext() = 0;
            virtual void captureStart() = 0;
            virtual void captureStop() = 0;

            virtual IplImage const* image() = 0;

            virtual void setAutoGain(bool value) = 0;
            virtual void setAutoShutter(bool value) = 0;
            virtual void setAutoSensorShutter(bool value) = 0;
            virtual void setAutoWhiteBalance(bool value) = 0;
            virtual void setMaxAutoShutter(double value) = 0;
            virtual void setExposureTime(double value) = 0;
            virtual void setPixelClock(unsigned int value) = 0;
            virtual void setHorizontalMirror(bool value) = 0;
            virtual void setVerticalMirror(bool value) = 0;
            virtual void setParameters(CameraParameters const& params) = 0;

            unsigned int width(){ return _width; }
            unsigned int height(){ return _height; }

        protected:
            unsigned int _width;
            unsigned int _height;
    };

}

#endif
