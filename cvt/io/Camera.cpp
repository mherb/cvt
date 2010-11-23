#include <cvt/io/Camera.h>

#ifdef APPLE
	#include <cvt/io/QTKitCamera.h>
#endif

#ifdef LINUX
	#include <cvt/io/V4L2Camera.h>
#endif

#include <cvt/io/DC1394Camera.h>

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
#ifdef APPLE
		for( size_t i = 0; i < QTKitCamera::count() ; i++ ){
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

		// dc1394 cameras
		for( size_t i = 0; i < DC1394Camera::count(); i++){
			Camera::_camInfos.push_back( CameraInfo() );
			DC1394Camera::cameraInfo( i, Camera::_camInfos.back() );
		}

		// ueye cameras
	}

	Camera * Camera::get( size_t index, size_t width, size_t height,
						  size_t fps, const IFormat & format )
	{
		if( index > Camera::_camInfos.size() ){
			throw CVTException( "Camera index out of bounds!" );
		}

		CameraInfo & camInfo = Camera::_camInfos[ index ];

		Camera * cam = NULL;

		switch( camInfo.type() ){
			case CAMERATYPE_UEYE:
				throw CVTException( "TODO IMPLEMENT UEYE camera handling" );
				break;
			case CAMERATYPE_DC1394:
				cam = new DC1394Camera( camInfo.index(), width, height, fps, format );
				break;
			case CAMERATYPE_V4L2:
#ifdef LINUX
				{
					const CameraMode & mode = camInfo.bestMatchingMode( format, width, height, fps );
					std::cout << "Selecting mode: " << mode << std::endl;
					cam = new V4L2Camera( camInfo.index(), mode );
				}
#endif
				break;
			case CAMERATYPE_QTKIT:
#ifdef APPLE
				cam = new QTKitCamera( Camera::camInfo.index(), width, height, fps, format );
#endif
				break;
			default:
				throw CVTException( "Unkown camera type" );
		}

		return cam;
	}
}
