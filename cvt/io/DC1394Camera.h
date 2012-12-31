/*
			CVT - Computer Vision Tools Library

 	 Copyright (c) 2012, Philipp Heise, Sebastian Klose

 	THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 	KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 	IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 	PARTICULAR PURPOSE.
 */
#ifndef DC1394CAMERA_H
#define DC1394CAMERA_H

#include <cvt/io/Camera.h>
#include <dc1394/dc1394.h>
#include <cvt/gfx/Image.h>

namespace cvt
{

	class DC1394Camera : public Camera
	{

		public:
			DC1394Camera( size_t camIndex, const CameraMode & mode );

			~DC1394Camera();

			void startCapture();
			void stopCapture();

			bool nextFrame( size_t timeout = 30 );
			const Image& frame() const;
			size_t width() const { return _width;}
			size_t height() const { return _height;}
			const IFormat & format() const { return _frame.format();}
            const String& identifier() const { return _identifier;}

			static size_t count();
			static void cameraInfo( size_t index, CameraInfo & info );

		private:
			void close();
			void init();
			void reset();
		
			/**
			 * \brief enable/disable automatic white-balance
			 */
			void enableWhiteBalanceAuto( bool enable );

			void getWhiteBalance(unsigned int* ubValue, unsigned int* vrValue);
			void setWhiteBalance(unsigned int ubValue, unsigned int vrValue);
			void enableShutterAuto(bool enable);
			void getShutter(unsigned int* value);
			void setShutter(unsigned int value);
			void enableGainAuto(bool enable);
			void enableIrisAuto(bool enable);
					
			/* find fitting dc1394 settings for given camera mode */
			void dcSettings( const CameraMode & mode );
		
			dc1394video_mode_t dcMode( const CameraMode & mode );
			
			int     _dmaBufNum;
			size_t  _camIndex;
			Image   _frame;
			size_t  _width;
			size_t  _height;
			size_t  _fps;

			bool                _capturing;
			dc1394_t*           _dcHandle;
			dc1394camera_t*     _camera;
			dc1394speed_t       _speed;
			dc1394video_mode_t  _mode;
			dc1394framerate_t   _framerate;
            String              _identifier;
	};

}

#endif
