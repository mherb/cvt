#ifndef TOOLS_MUEYEUSBCAMERA_H
#define TOOLS_MUEYEUSBCAMERA_H

#include <cvt/io/Camera.h>
#include <cvt/util/String.h>
#include <ueye.h>

namespace cvt
{

class UEyeUsbCamera : public Camera
    {
        public:
            UEyeUsbCamera( size_t camIndex, const CameraMode& mode );
            virtual ~UEyeUsbCamera();

            void			startCapture();
            void			stopCapture();
			void			nextFrame();
            const Image&	frame() const;

            static size_t	count();
			static void		cameraInfo( size_t index, CameraInfo & info );
			size_t			width() const;
			size_t			height() const;
			const IFormat&	format() const;
			const String&	identifier() const;
            
			void	setFramerate( double fps );
			void	setAutoGain( bool value );
            void	setAutoWhiteBalance( bool value );
			void	setWhiteBalanceOnce();
			void	setLiveMode( bool val );

        private:
            bool	initCam();
            void	open( const CameraMode & mode );
			void	initMemories( const CameraMode & mode );
			void	freeMemories();
            void	setAutoShutter( bool value );
            void	setAutoSensorShutter( bool value );
            void	setMaxAutoShutter( double value );
            void	setExposureTime( double value );
			void	exposureRange( double& min, double& max );
            void	setPixelClock( unsigned int value );
            void	setHorizontalMirror( bool value );
            void	setVerticalMirror( bool value );
            int		bufNumForAddr( const uint8_t * buffAddr ) const;
			void	setIdentifier();


			void	enableFreerun();
			void	disableFreerun();
			void	enableTriggered();
			void	enableEvents();
			void	disableEvents();

            int			_camIndex;

            // uEye variables
            HIDS		_camHandle;		// handle to camera
            INT			_width;			// width of image
            INT			_height;		// height of image
            INT			_stride;		// stride of image

			static const size_t	_numImageBuffers = 10;
			uint8_t*			_buffers[ _numImageBuffers ];
			INT					_bufferIds[ _numImageBuffers ];

			mutable Image	_frame;
			String	_identifier;
			
			enum UEyeRunMode
			{
				UEYE_MODE_TRIGGERED,
				UEYE_MODE_FREERUN
			};
			UEyeRunMode		_runMode;
    };

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
