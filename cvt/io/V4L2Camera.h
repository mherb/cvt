#ifndef V4L2CAMERA_H
#define V4L2CAMERA_H

#include <linux/videodev2.h>
#include <gfx/Image.h>

/**
 * @class V4L2Camera V4L2Camera
 * @brief Interface class for using V4L2 API to access (USB,...) cameras.
 * @author Sebastian Klose <kloses@in.tum.de>
 */



namespace cvt {

class V4L2Camera
{
	public:
		V4L2Camera(int camIndex = 0, unsigned int width=640, unsigned int height=480, unsigned int fps = 30);
		virtual ~V4L2Camera();

		void open();
		void close();
		void init();
		void captureStart();
		void captureStop();
		void captureNext();
		Image const * image();

		void updateAutoIrisExp();
		void setAutoIris(bool b);
		void setAutoExposure(bool b);
		void setExposureValue( unsigned int val );
		void setAutoFocus(bool b);
		void setAutoWhiteBalance(bool b);
		void setBacklightCompensation(bool b);

	private:
		unsigned int mWidth;
		unsigned int mHeight;
		unsigned int mFps;
		unsigned int mNumBuffers;
		int mCamIndex;
		bool mOpened;
		bool mCapturing;
		int mNextBuf;

		unsigned int mAbsExposureVal;
		bool mAutoExposure;
		bool mAutoIris;
		bool mAutoFocus;
		bool mAutoWhiteBalance;
		bool mBackLightCompensation;


		// the device file descriptor
		int mFd;

		// memory buffers for mmap frames
		void** mBuffers;

		// temporary buffer for "compressed" data
		Image* mFrame;

		/** V4L2 specific **/
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
		v4l2_ext_control * mExtControlsToSet;

		// private helper funcs ...
		void queryBuffers(bool unmap = false);
		void enqueueBuffers();
		void enumerateMenu();
		void showExtendedControls();
		void control(int field, int value);
		void extendedControl();
		void showCapabilities();


};

}

#endif
