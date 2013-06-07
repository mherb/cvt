/*
			CVT - Computer Vision Tools Library

 	 Copyright (c) 2012, Philipp Heise, Sebastian Klose

 	THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 	KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 	IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 	PARTICULAR PURPOSE.
 */
#include <cvt/io/V4L2Camera.h>
#include <cvt/io/FileSystem.h>

#include <iostream>

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

	V4L2Camera::V4L2Camera( size_t camIndex, const CameraMode & mode ) :
		_width(mode.width),
		_height(mode.height),
		_fps(mode.fps),
		_numBuffers(4),
		_camIndex(camIndex),
		_opened(false),
		_capturing(false),
		_nextBuf(-1),
		_fd(-1),
		_buffers(0),
		_frame(NULL),
		_format( mode.format ),
		_stamp( 0.0 ),
		_frameIdx( 0 ),
		_extControlsToSet(0),
		_autoExposure(false),
		_autoIris(false),
		_autoFocus(true),
		_autoWhiteBalance(true),
		_backLightCompensation(false),
		_absExposureVal(250),
		_identifier( "v4l2_" )
	{
		this->open();
		this->init();

		_identifier += camIndex;
	}

	V4L2Camera::~V4L2Camera()
	{
		if( _opened )
			this->close();
	}

	void V4L2Camera::open()
	{
		std::vector<String> devices;
		V4L2Camera::listDevices( devices );

		if( _camIndex >= devices.size() ){
			throw CVTException( "device index out of bounds!" );
		}

		_fd = ::open( devices[ _camIndex ].c_str(), O_RDWR | O_NONBLOCK );

		if( _fd == -1){
			String str( "Could not open device named \"");
		    str +=	devices[ _camIndex ];
		    str	+ "\"";
			throw CVTException( str.c_str() );
		}

		_opened = true;
	}

	void V4L2Camera::close()
	{
		for(unsigned int i= 0; i < _numBuffers; i++){
			if( ( _buffers[ i ] != MAP_FAILED ) && _buffer.length ){
				if( munmap( _buffers[ i ], _buffer.length ) < 0 ){
					throw CVTException( "Could not unmap buffer " + i );
				}
			}
		}

		if( _extControlsToSet != 0 )
			delete[] _extControlsToSet;

		if( _buffers != 0 )
			delete[] _buffers;

		if( _fd != -1 )
			::close(_fd);
		_opened = false;
	}

	void V4L2Camera::init()
	{
		// set the time code
		_timeCode.type = V4L2_TC_TYPE_30FPS;
		_timeCode.flags = V4L2_TC_FLAG_DROPFRAME;

		// ret value of ioctl ...
		int ret = 0;

		size_t reqWidth = _width;
		size_t reqHeight = _height;

		// initialize V4L2 stuff
		memset( &_fmt, 0, sizeof( struct v4l2_format ) );
		_fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		_fmt.fmt.pix.width = (int)reqWidth;
		_fmt.fmt.pix.height = (int)reqHeight;
		_fmt.fmt.pix.field = V4L2_FIELD_ANY;

		switch( _format.formatID ){
			case IFORMAT_YUYV_UINT8:
				_fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV;
				break;
			case IFORMAT_BGRA_UINT8:
				_fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_BGR32;
				break;
			case IFORMAT_RGBA_UINT8:
				_fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_RGB32;
				break;
			case IFORMAT_GRAY_UINT8:
				_fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_GREY;
				break;
			/*
			case IFORMAT_BAYER_RGGB_UINT8:
				_fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_SRGGB8;
				break;
			*/
			default:
				throw CVTException( "Format not supported!" );
				break;
		}

		ret = ioctl( _fd, VIDIOC_S_FMT, &_fmt);

		if( ret < 0 ) {
			throw CVTException( "Unable to set requested format!" );
		}

		_width = _fmt.fmt.pix.width;
		_height = _fmt.fmt.pix.height;

		if( _frame )
			delete _frame;
		_frame = new Image( _width, _height, _format );

		// set stream parameter (fps):
		_streamParameter.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		_streamParameter.parm.capture.timeperframe.numerator=1;
		_streamParameter.parm.capture.timeperframe.denominator=(int)_fps;

		ret = ioctl( _fd, VIDIOC_S_PARM, &_streamParameter );
		if( ret < 0 ){
			throw CVTException( "Could not set stream parameters!" );
		}

		if(_streamParameter.parm.capture.timeperframe.denominator != _fps){
			std::cout << "Requested framerate (" << _fps << ") not supported by device => using " <<
				_streamParameter.parm.capture.timeperframe.denominator << " fps" << std::endl;
		}

		// request the buffers:
		memset( &_requestBuffers, 0, sizeof(struct v4l2_requestbuffers) );
		_requestBuffers.count = (int)_numBuffers;
		_requestBuffers.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		_requestBuffers.memory = V4L2_MEMORY_MMAP;

		ret = ioctl(_fd, VIDIOC_REQBUFS, &_requestBuffers);
		if (ret < 0){
			throw CVTException("VIDIOC_REQBUFS - Unable to allocate buffers");
		}

		_buffers = new void*[_numBuffers];

		this->queryBuffers( false );
		this->enqueueBuffers();
	}

	void V4L2Camera::startCapture()
	{
		if(!_capturing){
			int type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
			int ret=0;

			ret = ioctl(_fd, VIDIOC_STREAMON, &type);
			if (ret < 0)
			{
				throw CVTException( "Could not start streaming!" );
			}
			_capturing = true;
		}
	}

	void V4L2Camera::stopCapture()
	{
		if( _capturing ){
			int type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
			int ret=0;

			ret = ioctl( _fd, VIDIOC_STREAMOFF, &type );
			if (ret < 0)
			{
				throw CVTException("Could not stop streaming!");
			}
			_capturing = false;
		}
	}

	bool V4L2Camera::nextFrame( size_t tout )
	{
		fd_set rdset;
		struct timeval timeout;

		//make sure we are capturing
		if(_capturing)
			startCapture();

		FD_ZERO(&rdset);
		FD_SET(_fd, &rdset);
		timeout.tv_sec = 0; // 1 sec timeout
		timeout.tv_usec = tout * 1000; // ms

		// select - wait for data or timeout
		int ret = select( _fd + 1, &rdset, NULL, NULL, &timeout );
		if (ret < 0){
			throw CVTException("Could not grab image (select error)");
		} else if( ret == 0 ) {
			// timeout!
			return false;
		} else if( ( ret > 0 ) && ( FD_ISSET(_fd, &rdset) ) ){
			memset(&_buffer, 0, sizeof(struct v4l2_buffer));
			_buffer.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
			_buffer.memory = V4L2_MEMORY_MMAP;			

			ret = ioctl( _fd, VIDIOC_DQBUF, &_buffer );
			if ( ret < 0 ) {
				throw CVTException( "Unable to dequeue buffer!" );
			}
		}

		// get frame from buffer
		if( _buffer.bytesused >= _frame->height() * _frame->width() * _format.bpp ) {
			size_t stride;
			uint8_t* ptrM;
			uint8_t* ptr;

			ptrM = ptr = _frame->map( &stride );
			size_t h = _frame->height();
			uint8_t * bufPtr = (uint8_t*)_buffers[ _buffer.index ];
			size_t bufStride = _frame->width() * _format.bpp;
			SIMD* simd = SIMD::instance();
			while( h-- ){
				simd->Memcpy( ptr, bufPtr, bufStride );
				ptr += stride;
				bufPtr += bufStride;
			}
			_frame->unmap( ptrM );
		}

		_frameIdx = _buffer.sequence;
		_stamp    = ( double )_buffer.timestamp.tv_sec +
					( double )( _buffer.timestamp.tv_usec ) / 1000000.0;

		ret = ioctl(_fd, VIDIOC_QBUF, &_buffer);
		if (ret < 0){
			throw CVTException("Unable to requeue buffer");
		}

		return true;
	}

	const Image & V4L2Camera::frame() const
	{
		assert(_frame != NULL);
		return *_frame;
	}

	void V4L2Camera::updateAutoIrisExp()
	{
		if(_autoExposure){
			if(_autoIris){
				V4L2Camera::control( _fd, V4L2_CID_EXPOSURE_AUTO, V4L2_EXPOSURE_AUTO );
			} else {
				V4L2Camera::control( _fd, V4L2_CID_EXPOSURE_AUTO, V4L2_EXPOSURE_APERTURE_PRIORITY );
			}
		} else {
			if(_autoIris) {
				V4L2Camera::control( _fd, V4L2_CID_EXPOSURE_AUTO, V4L2_EXPOSURE_SHUTTER_PRIORITY );
			} else {
				V4L2Camera::control( _fd, V4L2_CID_EXPOSURE_AUTO, V4L2_EXPOSURE_MANUAL );
			}

			V4L2Camera::control( _fd, V4L2_CID_EXPOSURE_ABSOLUTE, (int)_absExposureVal );
		}
	}

	void V4L2Camera::setAutoIris(bool b)
	{
		_autoIris = b;
		this->updateAutoIrisExp();
	}

	void V4L2Camera::setAutoExposure(bool b)
	{
		_autoExposure = b;
		this->updateAutoIrisExp();
	}

	void V4L2Camera::setExposureValue( unsigned int val )
	{
		_absExposureVal = val;
		V4L2Camera::control( _fd, V4L2_CID_EXPOSURE_ABSOLUTE, (int)_absExposureVal );
	}

	void V4L2Camera::setAutoFocus(bool b)
	{
		_autoFocus = b;
		V4L2Camera::control( _fd, V4L2_CID_FOCUS_AUTO, _autoFocus );
	}

	void V4L2Camera::setAutoWhiteBalance(bool b)
	{
		_autoWhiteBalance = b;
		V4L2Camera::control( _fd, V4L2_CID_AUTO_WHITE_BALANCE, _autoWhiteBalance);
	}

	void V4L2Camera::setBacklightCompensation(bool b)
	{
		_backLightCompensation = b;
		V4L2Camera::control( _fd, V4L2_CID_BACKLIGHT_COMPENSATION, _backLightCompensation );
	}

	void V4L2Camera::control(int fd, int field, int value)
	{
		struct v4l2_queryctrl queryctrl;

		memset( &queryctrl, 0, sizeof(queryctrl));
		// set the field
		queryctrl.id = field;

		if( ioctl( fd, VIDIOC_QUERYCTRL, &queryctrl) == -1 ){
			std::cout << "Error: This ioctl is not supported by the device" << std::endl;
		} else if( queryctrl.flags & V4L2_CTRL_FLAG_DISABLED ) {
			std::cout << "Field " << field << " is not supported" << std::endl;
		} else {
			struct v4l2_control control;
			memset( &control, 0, sizeof(control));
			control.id = field;
			control.value = value;

			if( ioctl(fd, VIDIOC_S_CTRL, &control) == -1){
				std::cout << "VIDIOC_S_CTRL ERROR: while setting value for field" << std::endl;
			}
		}
	}

	void V4L2Camera::extendedControl()
	{
		// set the vield in _extendedControls:
		memset(&_extendedControls, 0, sizeof(v4l2_ext_controls));
		_extendedControls.ctrl_class = V4L2_CTRL_CLASS_CAMERA;
		_extendedControls.count = 2;
		_extendedControls.controls = _extControlsToSet;

		if(_extControlsToSet != 0)
			delete[] _extControlsToSet;

		_extControlsToSet = new v4l2_ext_control[2];
		memset(&_extControlsToSet[0], 0, sizeof(v4l2_ext_control));
		memset(&_extControlsToSet[1], 0, sizeof(v4l2_ext_control));

		// set pan:
		_extControlsToSet[0].id = V4L2_CID_PAN_ABSOLUTE;
		//_extControlsToSet[0].value64 = 10*3600;

		_extControlsToSet[1].id = V4L2_CID_TILT_ABSOLUTE;
		//_extControlsToSet[1].value64 = -10*3600;

		if(ioctl(_fd, VIDIOC_G_EXT_CTRLS, &_extendedControls) == -1 ){
			std::cout << "Error setting extended controls ..." << std::endl;

			std::cout << "Error at index " << _extendedControls.error_idx << std::endl;
			perror("Problem:");
		} else {
			std::cout << "CURRENT VALUES:" << std::endl;
			std::cout << "PAN: " << _extControlsToSet[0].value << std::endl;
			std::cout << "TILT: " << _extControlsToSet[1].value << std::endl;
		}

	}

	void V4L2Camera::queryBuffers(bool unmap)
	{
		int ret=0;

		for (unsigned int i = 0; i < _numBuffers ; i++){
			// unmap old buffer
			if(unmap) {
				if( munmap(_buffers[i], _buffer.length) < 0 )
					perror("couldn't unmap buff");
			}

			memset(&_buffer, 0, sizeof(struct v4l2_buffer));
			_buffer.index = i;
			_buffer.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
			_buffer.flags = V4L2_BUF_FLAG_TIMECODE;
			_buffer.timecode = _timeCode;
			_buffer.timestamp.tv_sec = 0;//get frame as soon as possible
			_buffer.timestamp.tv_usec = 0;
			_buffer.memory = V4L2_MEMORY_MMAP;
			ret = ioctl(_fd, VIDIOC_QUERYBUF, &_buffer);
			if (ret < 0){
				perror("VIDIOC_QUERYBUF - Unable to query buffer");
				exit(0);
			}
			if (_buffer.length <= 0)
				std::cerr << "WARNING VIDIOC_QUERYBUF - buffer length is " << _buffer.length << std::endl;

			// map new buffer
			_buffers[i] = mmap( 0, _buffer.length, PROT_READ, MAP_SHARED, _fd, _buffer.m.offset );
			if (_buffers[i] == MAP_FAILED)
			{
				perror("Unable to map buffer");
				exit(0);
			}
		}

	}

	void V4L2Camera::enqueueBuffers()
	{
		int ret=0;
		for (unsigned int i = 0; i < _numBuffers; i++) {
			memset(&_buffer, 0, sizeof(struct v4l2_buffer));
			_buffer.index = i;
			_buffer.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
			_buffer.flags = V4L2_BUF_FLAG_TIMECODE;
			_buffer.timecode = _timeCode;
			_buffer.timestamp.tv_sec = 0;//get frame as soon as possible
			_buffer.timestamp.tv_usec = 0;
			_buffer.memory = V4L2_MEMORY_MMAP;
			ret = ioctl(_fd, VIDIOC_QBUF, &_buffer);
			if (ret < 0)
			{
				perror("VIDIOC_QBUF - Unable to queue buffer");
				exit(0);
			}
		}
	}

	size_t V4L2Camera::count()
	{
		std::vector<String> devs;
		listDevices( devs );
		return devs.size();
	}

	void V4L2Camera::cameraInfo( size_t index, CameraInfo & info )
	{
		std::vector<String> deviceNames;
		listDevices( deviceNames );

		if( index >= deviceNames.size() )
			throw CVTException( "No device with such index!" );

		int fd = ::open( deviceNames[ index ].c_str(), O_RDWR | O_NONBLOCK );
		if( fd < 0 ) {
			String str("Could not open device to get information: ");
	        str += deviceNames[ index ];
			throw CVTException( str.c_str() );
		}

		struct v4l2_capability caps;
		if( ioctl( fd, VIDIOC_QUERYCAP, &caps ) )
			throw CVTException( "ioctl failed!" );

		String name;
		name.sprintf( "%s", (char*)caps.card );
		info.setName( name );
		info.setIndex( index );
		info.setType( CAMERATYPE_V4L2 );

		struct v4l2_fmtdesc formatDescription;
		memset( &formatDescription, 0, sizeof( v4l2_fmtdesc ) );
		formatDescription.index = 0;
		formatDescription.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

		CameraMode currentMode;

		while( ioctl( fd, VIDIOC_ENUM_FMT, &formatDescription ) == 0 ){
			formatDescription.index++;
			bool validFormat = true;
			switch( formatDescription.pixelformat ){
				case V4L2_PIX_FMT_YUYV:
					currentMode.format = IFormat::YUYV_UINT8;
					break;
				/*
				   case V4L2_PIX_FMT_SRGGB8:
					currentMode.format = IFormat::BAYER_RGGB_UINT8;
					break;
				*/
				case V4L2_PIX_FMT_BGR32:
					currentMode.format = IFormat::BGRA_UINT8;
					break;
				case V4L2_PIX_FMT_RGB32:
					currentMode.format = IFormat::RGBA_UINT8;
					break;
				case V4L2_PIX_FMT_GREY:
					currentMode.format = IFormat::GRAY_UINT8;
					break;
				default:
					validFormat = false;
					std::cout << "\tFOURCC: " << char( formatDescription.pixelformat & 0xFF )
											<< char( ( formatDescription.pixelformat >> 8 ) & 0xFF )
											<< char( ( formatDescription.pixelformat >> 16 ) & 0xFF )
											<< char( ( formatDescription.pixelformat >> 24 ) & 0xFF ) << " available but not supported" << std::endl;
					break;
			}

			if( validFormat ){
				struct v4l2_frmsizeenum frameSize;
				memset( &frameSize, 0, sizeof( v4l2_frmsizeenum ) );
				frameSize.index = 0;
				frameSize.pixel_format = formatDescription.pixelformat;

				while( ioctl( fd, VIDIOC_ENUM_FRAMESIZES, &frameSize ) == 0 ){
					frameSize.index++;
					if( frameSize.type == V4L2_FRMSIZE_TYPE_DISCRETE ){
						currentMode.width = frameSize.discrete.width;
						currentMode.height = frameSize.discrete.height;

						// enumerate frame intervals:
						struct v4l2_frmivalenum fps;
						memset( &fps, 0, sizeof( v4l2_frmivalenum ) );
						fps.index = 0;
						fps.pixel_format = formatDescription.pixelformat;
						fps.width = frameSize.discrete.width;
						fps.height = frameSize.discrete.height;
						while( ioctl( fd, VIDIOC_ENUM_FRAMEINTERVALS, &fps ) == 0 ){
							fps.index++;
							if( fps.type == V4L2_FRMIVAL_TYPE_DISCRETE ){
								currentMode.fps = fps.discrete.denominator / fps.discrete.numerator;
								info.addMode( currentMode );
							}
						}
					}
				}
			}
		}

		::close( fd );
	}

	/* check for openable v4l devices in /sys/class/video4linux */
	void V4L2Camera::listDevices( std::vector<String> & devices )
	{
		std::vector<String> possibleDevs;
		FileSystem::ls( "/sys/class/video4linux", possibleDevs );

		struct v4l2_capability caps;
		String ss;
		for( size_t i = 0; i < possibleDevs.size(); i++ ){
			ss = "/dev/";
			ss += possibleDevs[ i ];

			int fd = ::open( ss.c_str(), O_RDWR | O_NONBLOCK );
			if( fd < 0 )
				continue;

			if( ioctl( fd, VIDIOC_QUERYCAP, &caps ) )
				continue;

			if( caps.capabilities & V4L2_CAP_VIDEO_CAPTURE )
				devices.push_back( ss );

			::close( fd );
		}
	}
}
