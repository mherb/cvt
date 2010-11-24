#ifndef TOOLS_MUEYEUSBCAMERA_H
#define TOOLS_MUEYEUSBCAMERA_H

#include <cvt/io/Camera.h>

#include <ueye.h>

namespace cvt
{

class UEyeUsbCamera : public Camera
    {
        public:
            UEyeUsbCamera( size_t camIndex, const CameraMode& mode );
            virtual ~UEyeUsbCamera();

            virtual void startCapture();
            virtual void stopCapture();
			virtual void nextFrame();
            virtual const Image & frame() const;

            static size_t count();
			static void cameraInfo( size_t index, CameraInfo & info );
        private:
            bool initCam();
            void open( const CameraMode & mode );
			virtual void setAutoGain( bool value );
            virtual void setAutoShutter( bool value );
            virtual void setAutoSensorShutter( bool value );
            virtual void setAutoWhiteBalance( bool value );
            virtual void setMaxAutoShutter( double value );
            virtual void setExposureTime( double value );
            virtual void setPixelClock( unsigned int value );
            virtual void setHorizontalMirror( bool value );
            virtual void setVerticalMirror( bool value );

            int _camIndex;

            // uEye variables
            HIDS	_camHandle;		// handle to camera
            INT		_width;			// width of video
            INT		_height;		// height of video
            INT		_bufferID;		// grabber memory - buffer ID
            char*	_bufferPtr;		// grabber memory - pointer to buffer

            Image _frame;
    };

	inline const Image & UEyeUsbCamera::frame() const
	{
		return _frame;
	}

}
#endif
