/*
            CVT - Computer Vision Tools Library

     Copyright (c) 2012, Philipp Heise, Sebastian Klose

    THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
    KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
    IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
    PARTICULAR PURPOSE.
 */
#include <cvt/io/Camera.h>

#ifdef APPLE
    #include <cvt/io/QTKitCamera.h>
#endif

#ifdef LINUX
	#include <cvt/io/V4L2Camera.h>
#endif
#ifdef UEYE_FOUND
	#include <cvt/io/UEyeUsbCamera.h>
#endif

#ifdef DC1394_FOUND
#include <cvt/io/DC1394Camera.h>
#endif

#ifdef OPENNI_FOUND
#include <cvt/io/OpenNICamera.h>
#endif

namespace cvt {

	std::vector<CameraInfo> Camera::_camInfos;

	Camera::Camera()
	{
	}

	Camera::~Camera()
	{
	}

	void Camera::updateInfo()
	{
		size_t count = 0;
#ifdef APPLE
		count = QTKitCamera::count();
		for( size_t i = 0; i < count; i++ ){
			Camera::_camInfos.push_back( CameraInfo() );
			QTKitCamera::cameraInfo( i, Camera::_camInfos.back() );
		}
#endif

#ifdef LINUX
		// all V4L2 devices
		for( size_t i = 0; i < V4L2Camera::count() ; i++ ){
			Camera::_camInfos.push_back( CameraInfo() );
			V4L2Camera::cameraInfo( i, Camera::_camInfos.back() );
		}
#endif

#ifdef DC1394_FOUND
		// dc1394 cameras
		for( size_t i = 0; i < DC1394Camera::count(); i++){
			Camera::_camInfos.push_back( CameraInfo() );
			DC1394Camera::cameraInfo( i, Camera::_camInfos.back() );
		}
#endif

		// ueye cameras
#ifdef UEYE_FOUND
		count = UEyeUsbCamera::count();
		for( size_t i = 0; i < count; i++){
			Camera::_camInfos.push_back( CameraInfo() );
			UEyeUsbCamera::cameraInfo( i, Camera::_camInfos.back() );
		}
#endif

#ifdef OPENNI_FOUND
		count = OpenNICamera::count();
		for( size_t i = 0; i < count; i++ ){
			Camera::_camInfos.push_back( CameraInfo() );
			OpenNICamera::cameraInfo( i, Camera::_camInfos.back() );
		}
#endif
	}

	Camera * Camera::get( size_t index, size_t width, size_t height,
						  size_t fps, const IFormat & format )
	{
		if( index >= Camera::_camInfos.size() ){
			throw CVTException( "Camera index out of bounds!" );
		}

		CameraInfo & camInfo = Camera::_camInfos[ index ];

		Camera * cam = NULL;

		const CameraMode & mode = camInfo.bestMatchingMode( format, width, height, fps );
		std::cout << "Selecting mode: " << mode << std::endl;

		switch( camInfo.type() ){
			case CAMERATYPE_UEYE:
#ifdef UEYE_FOUND
				cam = new UEyeUsbCamera( camInfo.index(), mode );
#endif
				break;
			case CAMERATYPE_DC1394:
#ifdef DC1394_FOUND
				cam = new DC1394Camera( camInfo.index(), mode );
#endif
				break;
			case CAMERATYPE_V4L2:
#ifdef LINUX
				cam = new V4L2Camera( camInfo.index(), mode );
#endif
				break;
			case CAMERATYPE_QTKIT:
#ifdef APPLE
				cam = new QTKitCamera( camInfo.index(), mode );
#endif
				break;
			case CAMERATYPE_OPENNI:
#ifdef OPENNI_FOUND
				cam = new OpenNICamera( camInfo.index(), mode );
#endif
				break;
			default:
				throw CVTException( "Unkown camera type" );
		}

		return cam;
	}

	Camera* Camera::get( size_t index, const CameraMode& mode )
	{
		return Camera::get( index, mode.width, mode.height, mode.fps, mode.format );
	}
}
