#include <cvt/io/Camera.h>

#ifdef APPLE
#include <cvt/io/QTKitCamera.h>
#endif

namespace cvt {
	
	std::vector<CameraInfo> Camera::_camInfos;
	
	void Camera::updateInfo()
	{
#ifdef LINUX
		std::cout << "getting all V4L2 devices" << std::endl;
#endif

#ifdef APPLE
		std::cout << "getting all QTKit devices" << std::endl;
		for( unsigned int i = 0; i < QTKitCamera::numberOfCameras(); i++ ){
			Camera::_camInfos.push_back( CameraInfo() );
			Camera::_camInfos.back()._type = CAMERATYPE_QTKIT;
			Camera::_camInfos.back()._index = i;
			QTKitCamera::nameForIndex( i, Camera::_camInfos.back()._name );
			
		}
#endif
		
		// dc1394 cameras
	}
	
	Camera* Camera::get( size_t index )
	{
		if( index > Camera::_camInfos.size() ){
			throw CVTException( "Camera index out of bounds!" );
		}
		return NULL;
	}
	
}
