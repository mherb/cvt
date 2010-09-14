#ifndef DC1394CAMERA_H
#define DC1394CAMERA_H

#include <dc1394/dc1394.h>
#include <cvt/gfx/Image.h>

namespace cvt
{

	class DC1394Camera
	{

		public:
			DC1394Camera(int camIndex = 0, unsigned int width=640, unsigned int height=480, unsigned int fps = 30, ImageChannelOrder order = CVT_BGRA);
			~DC1394Camera();

			void open();
			void close();
			void init();
			void reset();
			void captureStart();
			void captureStop();
			void captureNext();

			const Image* image();

			static int getNumberOfCameras();

		private:
			void enableWhiteBalanceAuto(bool enable);
			void getWhiteBalance(unsigned int* ubValue, unsigned int* vrValue);
			void setWhiteBalance(unsigned int ubValue, unsigned int vrValue);
			void enableShutterAuto(bool enable);
			void getShutter(unsigned int* value);
			void setShutter(unsigned int value);
			void enableGainAuto(bool enable);
			void enableIrisAuto(bool enable);

			int dmaBufNum;
			int mCamIndex;
			Image* mFrame;
			unsigned int mWidth;
			unsigned int mHeight;
			unsigned int mFps;
			ImageChannelOrder mIorder;

			bool capturing;
			dc1394_t* mDcHandle;
			dc1394camera_t* mCamera;
			dc1394speed_t mSpeed;
			dc1394video_mode_t mMode;
			dc1394framerate_t mFramerate;
	};

}

#endif
