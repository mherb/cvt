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

            void startCapture();
            void stopCapture();
			void nextFrame();
            const Image & frame() const;

            static size_t count();
			static void cameraInfo( size_t index, CameraInfo & info );
			size_t width() const;
			size_t height() const;
			const IFormat & format() const;
        private:
            bool	initCam();
            void	open( const CameraMode & mode );
			void	initMemories( const CameraMode & mode );
			void	freeMemories();
			void	setAutoGain( bool value );
            void	setAutoShutter( bool value );
            void	setAutoSensorShutter( bool value );
            void	setAutoWhiteBalance( bool value );
			void	setWhiteBalanceOnce();
            void	setMaxAutoShutter( double value );
            void	setExposureTime( double value );
            void	setPixelClock( unsigned int value );
            void	setHorizontalMirror( bool value );
            void	setVerticalMirror( bool value );
            void	setFramerate( double fps );
            int		bufNumForAddr( const uint8_t * buffAddr ) const;

            int _camIndex;

            // uEye variables
            HIDS		_camHandle;		// handle to camera
            INT			_width;			// width of image
            INT			_height;		// height of image
            INT			_stride;		// stride of imagte

			static const size_t	_numImageBuffers = 4;
			uint8_t*			_buffers[ _numImageBuffers ];
			INT					_bufferIds[ _numImageBuffers ];

			Image _frame;
    };

	inline const Image & UEyeUsbCamera::frame() const
	{
		return _frame;
	}

	inline size_t UEyeUsbCamera::width() const
	{
		return _frame.width();
	}

	inline size_t UEyeUsbCamera::height() const
	{
		return _frame.height();
	}

	inline const IFormat & UEyeUsbCamera::format() const
	{
		return _frame.format();
	}
}
#endif
