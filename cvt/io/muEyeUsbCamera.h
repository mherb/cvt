#ifndef TOOLS_MUEYEUSBCAMERA_H
#define TOOLS_MUEYEUSBCAMERA_H

#include "io/AbstractCamera.hpp"

#include <ueye.h>

namespace cvt
{

class muEyeUsbCamera : public AbstractCamera
    {
        public:
            muEyeUsbCamera(unsigned int camIndex,
                          CameraParameters const& params);

            virtual ~muEyeUsbCamera();

            virtual void captureStart();
            virtual void captureStop();
            virtual void captureNext();

            virtual IplImage const* image();

            virtual void setAutoGain(bool value);

            virtual void setAutoShutter(bool value);

            virtual void setAutoSensorShutter(bool value);

            virtual void setAutoWhiteBalance(bool value);

            virtual void setMaxAutoShutter(double value);

            virtual void setExposureTime(double value);

            virtual void setPixelClock(unsigned int value);

            virtual void setHorizontalMirror(bool value);

            virtual void setVerticalMirror(bool value);

            virtual void setParameters(CameraParameters const& params);

            static int availableCameras();
        private:
            void initCam();
            void updateParameters();
            void setOfflineParameters();
            void open(CameraParameters const& params);

            int mCamIndex;

            // uEye varibles
            HIDS	m_hCam;			// handle to camera
            INT		m_Ret;			// return value of uEye SDK functions
            INT		m_nColorMode;	// Y8/RGB16/RGB24/REG32
            INT		m_nBitsPerPixel;// number of bits needed store one pixel
            INT		m_nSizeX;		// width of video
            INT		m_nSizeY;		// height of video
            INT		m_lMemoryId;	// grabber memory - buffer ID
            char*	m_pcImageMemory;// grabber memory - pointer to buffer
            INT     m_nRenderMode;  // render  mode

            IplImage * img;

            // status flag
            INT nRet;

    };

}
#endif
