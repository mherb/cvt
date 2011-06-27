#ifndef CVT_V4L2CAMERA_H
#define CVT_V4L2CAMERA_H

#include <cvt/io/Camera.h>

#include <linux/videodev2.h>
#include <gfx/Image.h>

namespace cvt {

	class V4L2Camera : public Camera
	{
		public:
			V4L2Camera( size_t camIndex,
						const CameraMode & mode );

			virtual ~V4L2Camera();

			size_t width() const;
			size_t height() const;
			const IFormat & format() const;
			const Image & frame() const;
			void nextFrame();
			void startCapture();
			void stopCapture();

			void updateAutoIrisExp();
			void setAutoIris(bool b);
			void setAutoExposure(bool b);
			void setExposureValue( unsigned int val );
			void setAutoFocus(bool b);
			void setAutoWhiteBalance(bool b);
			void setBacklightCompensation(bool b);

			static size_t count();
			static void cameraInfo( size_t index, CameraInfo & info );

		private:
			size_t _width;
			size_t _height;
			size_t _fps;
			size_t _numBuffers;
			size_t _camIndex;
			bool _opened;
			bool _capturing;
			int _nextBuf;

			// the device file descriptor
			int _fd;

			// memory buffers for mmap frames
			void** _buffers;

			Image* _frame;
			IFormat _format;

			/** V4L2 specific **/
			v4l2_ext_control *	_extControlsToSet;
			v4l2_format			_fmt;
			v4l2_streamparm		_streamParameter;
			v4l2_requestbuffers _requestBuffers;
			v4l2_buffer			_buffer;
			v4l2_timecode		_timeCode;
			v4l2_queryctrl		_queryCtrl;
			v4l2_querymenu		_queryMenu;
			v4l2_input			_input;
			v4l2_control		_control;
			v4l2_ext_controls	_extendedControls;

			bool	_autoExposure;
			bool	_autoIris;
			bool	_autoFocus;
			bool	_autoWhiteBalance;
			bool	_backLightCompensation;
			size_t	_absExposureVal;


			// private helper funcs ...
			void open();
			void close();
			void init();
			void queryBuffers( bool unmap = false );
			void enqueueBuffers();
			void extendedControl();
			static void control( int fd, int field, int value );
			static void listDevices( std::vector<String> & devices );
	};

	inline size_t V4L2Camera::width() const
	{
		return _width;
	}

	inline size_t V4L2Camera::height() const
	{
		return _height;
	}

	inline const IFormat & V4L2Camera::format() const
	{
		return _format;
	}

}

#endif
