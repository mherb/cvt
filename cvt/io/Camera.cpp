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
		for( unsigned int i = 0; i < QTKitCamera::count(); i++ ){
			Camera::_camInfos.push_back( CameraInfo() );
			Camera::_camInfos.back()._type = CAMERATYPE_QTKIT;
			Camera::_camInfos.back()._index = i;
			QTKitCamera::nameForIndex( i, Camera::_camInfos.back()._name );			
		}
#endif
#ifdef LINUX
		// all V4L2 devices
#endif
				
		// dc1394 cameras
		
		// ueye cameras
	}
	
	Camera * Camera::get( size_t index, size_t width, size_t height, 
						  size_t fps, cvt::IOrder order, cvt::IType type )
	{
		if( index > Camera::_camInfos.size() ){
			throw CVTException( "Camera index out of bounds!" );
		}
		
		Camera * cam = NULL;
		
		switch( Camera::_camInfos[ index ].type() ){
			case CAMERATYPE_UEYE:
				throw CVTException( "TODO IMPLEMENT UEYE camera handling" );
				break;
			case CAMERATYPE_DC1394:
				throw CVTException( "TODO IMPLEMENT DC1394 camera handling" );
				break;
			case CAMERATYPE_V4L2:
				throw CVTException( "TODO IMPLEMENT DC1394 camera handling" );
				break;
			case CAMERATYPE_QTKIT:
#ifdef APPLE
				cam = new QTKitCamera( Camera::_camInfos[ index ].index(), width, height, fps, order, type );
#endif
				break;
			default:
				throw CVTException( "Unkown camera type" );				
		}
		
		return cam;
	}
	
}
