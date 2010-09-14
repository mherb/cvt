#include "V4L2Camera.h"

#include <iostream>
#include <string>
#include <sstream>

#include <cstdlib>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <asm/types.h>
#include <errno.h>
#include <linux/videodev2.h>
#include <stdio.h>

#include "util/Exception.h"
#include "math/Math.h"

namespace cvt {

	static inline void pixelYUV2BGRA( uint32_t yuyv, uint32_t& p1, uint32_t& p2 )
	{
		int r, g, b;
		int y0, y1, u, v;

		v = ( yuyv >> 24 ) - 128;
		y1 = ( ( yuyv >> 16 ) & 0xff );
		u = ( ( yuyv >> 8 ) & 0xff ) - 128;
		y0 = ( ( yuyv ) & 0xff );
		r = ((v*1436) >> 10);
		g = ((u*352 + v*731) >> 10);
		b = ((u*1814) >> 10);

		// clamp the values
		p1 = 0xff000000;
		p1 |= Math::clamp( y0 + r, 0, 255 ) << 16;
		p1 |= Math::clamp( y0 - g, 0, 255 ) << 8;
		p1 |= Math::clamp( y0 + b, 0, 255 );
		p2 = 0xff000000;
		p2 |= Math::clamp( y1 + r, 0, 255 ) << 16;
		p2 |= Math::clamp( y1 - g, 0, 255 ) << 8;
		p2 |= Math::clamp( y1 + b, 0, 255 );
	}

	static inline void pixelYUV2RGBA( uint32_t yuyv, uint32_t& p1, uint32_t& p2 )
	{
		int r, g, b;
		int y0, y1, u, v;

		v = ( yuyv >> 24 ) - 128;
		y1 = ( ( yuyv >> 16 ) & 0xff );
		u = ( ( yuyv >> 8 ) & 0xff ) - 128;
		y0 = ( ( yuyv ) & 0xff );
		r = ((v*1436) >> 10);
		g = ((u*352 + v*731) >> 10);
		b = ((u*1814) >> 10);

		// clamp the values
		p1 = 0xff000000;
		p1 |= Math::clamp( y0 + r, 0, 255 );
		p1 |= Math::clamp( y0 - g, 0, 255 ) << 8;
		p1 |= Math::clamp( y0 + b, 0, 255 ) << 16;
		p2 = 0xff000000;
		p2 |= Math::clamp( y1 + r, 0, 255 );
		p2 |= Math::clamp( y1 - g, 0, 255 ) << 8;
		p2 |= Math::clamp( y1 + b, 0, 255 ) << 16;
	}

	static void YUYV2COLOR( uint8_t* dst, uint8_t* src, size_t w, size_t h, size_t stridedst, size_t stridesrc, ImageChannelOrder order )
	{
		size_t l, c;

		l = h;
		if( order == CVT_RGBA ) {
			uint32_t *s;
			uint32_t *d;
			uint32_t p1, p2;

			while( l-- ) {
				s = ( uint32_t* ) src;
				d = ( uint32_t* ) dst;
				c = w >> 1;
				while (c--) {
					pixelYUV2RGBA( *s++, p1, p2);
					*d++ = p1;
					*d++ = p2;
				}
				src += stridesrc;
				dst += stridedst;
			}
		} else if( order == CVT_BGRA ) {
			uint32_t *s;
			uint32_t *d;
			uint32_t p1, p2;

			while( l-- ) {
				s = ( uint32_t* ) src;
				d = ( uint32_t* ) dst;
				c = w >> 1;
				while (c--) {
					pixelYUV2BGRA( *s++, p1, p2);
					*d++ = p1;
					*d++ = p2;
				}
				src += stridesrc;
				dst += stridedst;
			}
		} else if( order == CVT_GRAY ) {
			uint32_t *s;
			uint8_t *d;

			while( l-- ) {
				s = ( uint32_t* ) src;
				d = dst;
				c = w >> 1;
				while (c--) {
					*d++ = *s & 0xff;
					*d++ = ( *s++ >> 16 ) & 0xff;
				}
				src += stridesrc;
				dst += stridedst;
			}
		} else if( order == CVT_GRAYALPHA ) {
			uint32_t *s;
			uint8_t *d;

			while( l-- ) {
				s = ( uint32_t* ) src;
				d = dst;
				c = w >> 1;
				while (c--) {
					*d++ = *s & 0xff;
					*d++ = 255;
					*d++ = ( *s++ >> 16 ) & 0xff;
					*d++ = 255;
				}
				src += stridesrc;
				dst += stridedst;
			}
		}
	}


	V4L2Camera::V4L2Camera(int camIndex, unsigned int width, unsigned int height, unsigned int fps, ImageChannelOrder order) :
		mWidth(width),
		mHeight(height),
		mFps(fps),
		mNumBuffers(2),
		mCamIndex(camIndex),
		mOpened(false),
		mCapturing(false),
		mNextBuf(-1),
		mFd(-1),
		mBuffers(0),
		mFrame(NULL),
		mImgorder( order ),
		mExtControlsToSet(0),
		mAutoExposure(false),
		mAutoIris(false),
		mAutoFocus(true),
		mAutoWhiteBalance(true),
		mBackLightCompensation(false),
		mAbsExposureVal(250)
	{
	}

	V4L2Camera::~V4L2Camera()
	{
		if(mOpened)
			this->close();
	}

	void V4L2Camera::open()
	{
		std::stringstream stream;
		stream << "/dev/video" << mCamIndex;
		std::string videoDevice = stream.str();

		// TODO: check if the filename exists

		mFd = ::open(videoDevice.c_str(), O_RDWR);

		if( mFd == -1){
			throw CVTException("Could not open device named \"" + videoDevice + "\"");
		}

		//    this->showCapabilities();
		//    this->showExtendedControls();
		mOpened = true;
	}

	void V4L2Camera::close()
	{
		for(unsigned int i= 0; i < mNumBuffers; i++){
			if( (mBuffers[i] != MAP_FAILED) && mBuffer.length ){
				if( munmap( mBuffers[i], mBuffer.length ) < 0 ){
					throw CVTException("Could not unmap buffer " + i);
				}
			}
		}

		if(mExtControlsToSet != 0)
			delete[] mExtControlsToSet;

		if( mBuffers != 0 )
			delete[] mBuffers;

		if(mFd != -1)
			::close(mFd);
		mOpened = false;
	}

	void V4L2Camera::init()
	{
		// set the time code
		mTimeCode.type = V4L2_TC_TYPE_30FPS;
		mTimeCode.flags = V4L2_TC_FLAG_DROPFRAME;

		// ret value of ioctl ...
		int ret = 0;

		unsigned int reqWidth = mWidth;
		unsigned int reqHeight = mHeight;

		// initialize V4L2 stuff
		memset(&mFmt, 0, sizeof(struct v4l2_format));
		mFmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		mFmt.fmt.pix.width = reqWidth;
		mFmt.fmt.pix.height = reqHeight;
		mFmt.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV;
		mFmt.fmt.pix.field = V4L2_FIELD_ANY;

		ret = ioctl( mFd, VIDIOC_S_FMT, &mFmt);

		if(ret < 0){
			std::cerr << "VIDIOC_S_FORMAT - Unable to set format!" << std::endl;
		}

		// check if the requested format is available:
		if( mFmt.fmt.pix.width != reqWidth || mFmt.fmt.pix.height != reqHeight ){
			std::cout << "Requested Size not supported: " << reqWidth << "x" << reqHeight << std::endl;
			std::cout << "Switching to supported Size: " << mFmt.fmt.pix.width << "x" << mFmt.fmt.pix.height << std::endl;
		}
		mWidth = mFmt.fmt.pix.width;
		mHeight = mFmt.fmt.pix.height;

		// now alloc the tmpimage: YUYV needs 3/2 bytes per pixel => allocate two!?!?!

		if( mFrame )
			delete mFrame;
		mFrame = new Image( mWidth, mHeight, mImgorder );

		// set stream parameter (fps):
		mStreamParameter.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		mStreamParameter.parm.capture.timeperframe.numerator=1;
		mStreamParameter.parm.capture.timeperframe.denominator=mFps;

		ret = ioctl( mFd, VIDIOC_S_PARM, &mStreamParameter);

		if(ret < 0){
			std::cerr << "VIDIOC_S_PARM error: Unable to set FPS to " << mFps << std::endl;
		}
		if(mStreamParameter.parm.capture.timeperframe.denominator != mFps){
			std::cout << "Requested framerate (" << mFps << ") not supported by device => using " <<
				mStreamParameter.parm.capture.timeperframe.denominator << " fps" << std::endl;
		}

		// request the buffers:
		memset(&mRequestBuffers, 0, sizeof(struct v4l2_requestbuffers));
		mRequestBuffers.count = mNumBuffers;
		mRequestBuffers.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		mRequestBuffers.memory = V4L2_MEMORY_MMAP;

		ret = ioctl(mFd, VIDIOC_REQBUFS, &mRequestBuffers);
		if (ret < 0)
		{
			throw CVTException("VIDIOC_REQBUFS - Unable to allocate buffers");
		}

		mBuffers = new void*[mNumBuffers];

		this->queryBuffers(false);
		this->enqueueBuffers();
	}

	void V4L2Camera::captureStart()
	{
		if(!mCapturing){
			int type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
			int ret=0;

			ret = ioctl(mFd, VIDIOC_STREAMON, &type);
			if (ret < 0)
			{
				perror("VIDIOC_STREAMON - Unable to start capture");
				std::cout << "Error starting capturing" << std::endl;
			}
			mCapturing = true;
		}
	}

	void V4L2Camera::captureStop()
	{
		if(mCapturing){
			int type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
			int ret=0;

			ret = ioctl(mFd, VIDIOC_STREAMOFF, &type);
			if (ret < 0)
			{
				perror("VIDIOC_STREAMOFF - Unable to stop capture");
				throw CVTException("Error stopping capturing");
			}
			mCapturing = false;
		}
	}

	void V4L2Camera::captureNext()
	{
		fd_set rdset;
		struct timeval timeout;

		//make sure we are capturing
		if(mCapturing)
			captureStart();

		FD_ZERO(&rdset);
		FD_SET(mFd, &rdset);
		timeout.tv_sec = 2; // 1 sec timeout
		timeout.tv_usec = 0;

		// select - wait for data or timeout
		int ret = select(mFd + 1, &rdset, NULL, NULL, &timeout);
		if (ret < 0){
			perror(" Could not grab image (select error)");
			throw CVTException("Could not grab image (select error)");
		} else if (ret == 0) {
			perror(" Could not grab image (select timeout)");
			throw CVTException("Could not grab image (select timeout)");
		} else if ((ret > 0) && (FD_ISSET(mFd, &rdset))){
			memset(&mBuffer, 0, sizeof(struct v4l2_buffer));
			mBuffer.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
			mBuffer.memory = V4L2_MEMORY_MMAP;

			ret = ioctl(mFd, VIDIOC_DQBUF, &mBuffer);
			if (ret < 0) {
				perror("VIDIOC_DQBUF - Unable to dequeue buffer ");
				throw CVTException("Unable to dequeue buffer!");
			}
		}

		// decompress frame
		if( mBuffer.bytesused >= mFrame->height() * mFrame->width() * 2 ) {
			YUYV2COLOR( mFrame->data(), ( uint8_t* ) mBuffers[mBuffer.index], mFrame->width(), mFrame->height(), mFrame->stride(), 2 * mFrame->width(), mImgorder );
		}

		ret = ioctl(mFd, VIDIOC_QBUF, &mBuffer);
		if (ret < 0){
			perror("VIDIOC_QBUF - Unable to requeue buffer");
			throw CVTException("Unable to requeue buffer");
		}
	}

	const Image* V4L2Camera::image()
	{
		assert(mFrame != NULL);
		return mFrame;
	}

	void V4L2Camera::updateAutoIrisExp()
	{
		if(mAutoExposure){
			if(mAutoIris){
				this->control(V4L2_CID_EXPOSURE_AUTO, V4L2_EXPOSURE_AUTO);
			} else {
				this->control(V4L2_CID_EXPOSURE_AUTO, V4L2_EXPOSURE_APERTURE_PRIORITY);
			}
		} else {
			if(mAutoIris) {
				this->control(V4L2_CID_EXPOSURE_AUTO, V4L2_EXPOSURE_SHUTTER_PRIORITY);
			} else {
				this->control(V4L2_CID_EXPOSURE_AUTO, V4L2_EXPOSURE_MANUAL);
			}

			this->control(V4L2_CID_EXPOSURE_ABSOLUTE, mAbsExposureVal);
		}
	}

	void V4L2Camera::setAutoIris(bool b)
	{
		mAutoIris = b;
		this->updateAutoIrisExp();
	}

	void V4L2Camera::setAutoExposure(bool b)
	{
		mAutoExposure = b;
		this->updateAutoIrisExp();
	}

	void V4L2Camera::setExposureValue( unsigned int val )
	{
		mAbsExposureVal = val;
		this->control(V4L2_CID_EXPOSURE_ABSOLUTE, mAbsExposureVal);
	}

	void V4L2Camera::setAutoFocus(bool b)
	{
		mAutoFocus = b;
		this->control(V4L2_CID_FOCUS_AUTO, mAutoFocus);
	}

	void V4L2Camera::setAutoWhiteBalance(bool b)
	{
		mAutoWhiteBalance = b;
		this->control(V4L2_CID_AUTO_WHITE_BALANCE, mAutoWhiteBalance);
	}

	void V4L2Camera::setBacklightCompensation(bool b)
	{
		mBackLightCompensation = b;
		this->control(V4L2_CID_BACKLIGHT_COMPENSATION, mBackLightCompensation);
	}

	void V4L2Camera::control(int field, int value)
	{
		struct v4l2_queryctrl queryctrl;

		memset( &queryctrl, 0, sizeof(queryctrl));
		// set the field
		queryctrl.id = field;

		if( ioctl(mFd, VIDIOC_QUERYCTRL, &queryctrl) == -1 ){
			std::cout << "Error: This ioctl is not supported by the device" << std::endl;
		} else if( queryctrl.flags & V4L2_CTRL_FLAG_DISABLED ) {
			std::cout << "Field " << field << " is not supported" << std::endl;
		} else {
			struct v4l2_control control;
			memset( &control, 0, sizeof(control));
			control.id = field;
			control.value = value;

			if( ioctl(mFd, VIDIOC_S_CTRL, &control) == -1){
				std::cout << "VIDIOC_S_CTRL ERROR: while setting value for field" << std::endl;
			}
		}
	}

	void V4L2Camera::extendedControl()
	{
		// set the vield in mExtendedControls:
		memset(&mExtendedControls, 0, sizeof(v4l2_ext_controls));
		mExtendedControls.ctrl_class = V4L2_CTRL_CLASS_CAMERA;
		mExtendedControls.count = 2;
		mExtendedControls.controls = mExtControlsToSet;

		if(mExtControlsToSet != 0)
			delete[] mExtControlsToSet;

		mExtControlsToSet = new v4l2_ext_control[2];
		memset(&mExtControlsToSet[0], 0, sizeof(v4l2_ext_control));
		memset(&mExtControlsToSet[1], 0, sizeof(v4l2_ext_control));

		// set pan:
		mExtControlsToSet[0].id = V4L2_CID_PAN_ABSOLUTE;
		//mExtControlsToSet[0].value64 = 10*3600;

		mExtControlsToSet[1].id = V4L2_CID_TILT_ABSOLUTE;
		//mExtControlsToSet[1].value64 = -10*3600;

		if(ioctl(mFd, VIDIOC_G_EXT_CTRLS, &mExtendedControls) == -1 ){
			std::cout << "Error setting extended controls ..." << std::endl;

			std::cout << "Error at index " << mExtendedControls.error_idx << std::endl;
			perror("Problem:");
		} else {
			std::cout << "CURRENT VALUES:" << std::endl;
			std::cout << "PAN: " << mExtControlsToSet[0].value << std::endl;
			std::cout << "TILT: " << mExtControlsToSet[1].value << std::endl;
		}

	}

	void V4L2Camera::showCapabilities()
	{
		// TODO: iterate over control fields of the device and display the results
		/* list all available formats */
		for (int i = 0; ; i++) {
			struct v4l2_fmtdesc fmtdesc;

			fmtdesc.index = i;
			fmtdesc.type  = V4L2_BUF_TYPE_VIDEO_CAPTURE;
			if (ioctl(mFd, VIDIOC_ENUM_FMT, &fmtdesc) < 0) {
				break;
			}
			printf("Format: %s\n", fmtdesc.description);
		}

		this->showExtendedControls();

	}

	void V4L2Camera::queryBuffers(bool unmap)
	{
		int ret=0;

		for (unsigned int i = 0; i < mNumBuffers ; i++){
			// unmap old buffer
			if(unmap)
				if( munmap(mBuffers[i], mBuffer.length) < 0 )
					perror("couldn't unmap buff");

			memset(&mBuffer, 0, sizeof(struct v4l2_buffer));
			mBuffer.index = i;
			mBuffer.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
			mBuffer.flags = V4L2_BUF_FLAG_TIMECODE;
			mBuffer.timecode = mTimeCode;
			mBuffer.timestamp.tv_sec = 0;//get frame as soon as possible
			mBuffer.timestamp.tv_usec = 0;
			mBuffer.memory = V4L2_MEMORY_MMAP;
			ret = ioctl(mFd, VIDIOC_QUERYBUF, &mBuffer);
			if (ret < 0){
				perror("VIDIOC_QUERYBUF - Unable to query buffer");
				exit(0);
			}
			if (mBuffer.length <= 0)
				std::cerr << "WARNING VIDIOC_QUERYBUF - buffer length is " << mBuffer.length << std::endl;

			// map new buffer
			mBuffers[i] = mmap( 0, mBuffer.length, PROT_READ, MAP_SHARED, mFd, mBuffer.m.offset );
			if (mBuffers[i] == MAP_FAILED)
			{
				perror("Unable to map buffer");
				exit(0);
			}
		}

	}

	void V4L2Camera::enqueueBuffers()
	{
		int ret=0;
		for (unsigned int i = 0; i < mNumBuffers; i++) {
			memset(&mBuffer, 0, sizeof(struct v4l2_buffer));
			mBuffer.index = i;
			mBuffer.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
			mBuffer.flags = V4L2_BUF_FLAG_TIMECODE;
			mBuffer.timecode = mTimeCode;
			mBuffer.timestamp.tv_sec = 0;//get frame as soon as possible
			mBuffer.timestamp.tv_usec = 0;
			mBuffer.memory = V4L2_MEMORY_MMAP;
			ret = ioctl(mFd, VIDIOC_QBUF, &mBuffer);
			if (ret < 0)
			{
				perror("VIDIOC_QBUF - Unable to queue buffer");
				exit(0);
			}
		}
	}

	void V4L2Camera::enumerateMenu()
	{
		std::cout << "  Menu items: " << std::endl;

		memset (&mQueryMenu, 0, sizeof (mQueryMenu));
		mQueryMenu.id = mQueryCtrl.id;

		for (mQueryMenu.index = mQueryCtrl.minimum;
			 static_cast<int>(mQueryMenu.index) <= mQueryCtrl.maximum;
			 mQueryMenu.index++) {
			if (0 == ioctl (mFd, VIDIOC_QUERYMENU, &mQueryMenu)) {
				std::cout << "\t" << mQueryMenu.name << std::endl;
			} else {
				perror ("VIDIOC_QUERYMENU");
				throw CVTException("Error during menu query!!!");
			}
		}
	}

	void V4L2Camera::showExtendedControls()
	{
		memset(&mQueryCtrl, 0, sizeof (mQueryCtrl));

		mQueryCtrl.id = V4L2_CTRL_FLAG_NEXT_CTRL;

		std::cout << "************ EXTENDED CTRLS ************" << std::endl;
		while( ioctl( mFd, VIDIOC_QUERYCTRL, &mQueryCtrl ) == 0 ){
			std::cout << "Control " << mQueryCtrl.name << std::endl;

			if (mQueryCtrl.type == V4L2_CTRL_TYPE_MENU)
				enumerateMenu();

			mQueryCtrl.id |= V4L2_CTRL_FLAG_NEXT_CTRL;
		}
		std::cout << "****************** END *****************" << std::endl;
	}

}
