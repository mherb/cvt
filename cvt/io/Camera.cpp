#include <cvt/io/Camera.h>

#ifdef APPLE
#include <cvt/io/QTKitCamera.h>
#endif

#include <cvt/io/DC1394Camera.h>

namespace cvt {
	
	std::vector<CameraInfo> Camera::_camInfos;
	
	void Camera::updateInfo()
	{
		size_t cams = 0;
#ifdef APPLE		
		cams = QTKitCamera::count();
		for( size_t i = 0; i < cams; i++ ){
			Camera::_camInfos.push_back( CameraInfo() );
			QTKitCamera::cameraInfo( i, Camera::_camInfos.back() );			
		}
#endif
#ifdef LINUX
		// all V4L2 devices
#endif
				
		// dc1394 cameras
		cams = DC1394Camera::count();
		for( size_t i = 0; i < cams; i++){
			Camera::_camInfos.push_back( CameraInfo() );
			DC1394Camera::cameraInfo( i, Camera::_camInfos.back() );
		}
		
		
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
				cam = new DC1394Camera( Camera::_camInfos[ index ].index(), width, height, fps, order, type );
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
