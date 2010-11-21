#ifndef CVT_V4L2CAMERA_H
#define CVT_V4L2CAMERA_H

#include <cvt/io/Camera.h>

#include <linux/videodev2.h>
#include <gfx/Image.h>

namespace cvt {

	class V4L2Camera : public Camera
	{
		public:
			V4L2Camera( size_t camIndex = 0,
					    size_t width=640,
					    size_t height=480,
					    size_t fps = 30,
						const IFormat & format = IFormat::BGRA_UINT8 );

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
			size_t mWidth;
			size_t mHeight;
			size_t mFps;
			size_t mNumBuffers;
			size_t mCamIndex;
			bool mOpened;
			bool mCapturing;
			int mNextBuf;

			// the device file descriptor
			int mFd;

			// memory buffers for mmap frames
			void** mBuffers;

			Image* mFrame;
			const IFormat & mFormat;

			/** V4L2 specific **/
			v4l2_ext_control * mExtControlsToSet;
			v4l2_format mFmt;
			v4l2_streamparm mStreamParameter;
			v4l2_requestbuffers mRequestBuffers;
			v4l2_buffer mBuffer;
			v4l2_timecode mTimeCode;
			v4l2_queryctrl mQueryCtrl;
			v4l2_querymenu mQueryMenu;
			v4l2_input mInput;
			v4l2_control mControl;
			v4l2_ext_controls mExtendedControls;

			bool mAutoExposure;
			bool mAutoIris;
			bool mAutoFocus;
			bool mAutoWhiteBalance;
			bool mBackLightCompensation;
			unsigned int mAbsExposureVal;


			// private helper funcs ...
			void open();
			void close();
			void init();
			void queryBuffers( bool unmap = false );
			void enqueueBuffers();
			void enumerateMenu();
			void showExtendedControls();
			void extendedControl();
			void showCapabilities();
			static void control( int fd, int field, int value );
			static void listDevices( std::vector<std::string> & devices );
	};

	inline size_t V4L2Camera::width() const
	{
		return mWidth;
	}

	inline size_t V4L2Camera::height() const
	{
		return mHeight;
	}

	inline const IFormat & V4L2Camera::format() const
	{
		return mFormat;
	}

}

#endif
