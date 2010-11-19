#ifndef CVT_CAMERA_H
#define CVT_CAMERA_H

#include <vector>

#include <cvt/io/VideoInput.h>
#include <cvt/io/CameraInfo.h>
#include <cvt/util/Exception.h>

namespace cvt {
	class Camera : public VideoInput
	{
		public:
			/* number of available cameras */
			static size_t count();
			static const CameraInfo & info( size_t index );
			static void updateInfo();
			static Camera* get( size_t index, size_t width = 640, size_t height = 480,
							    size_t fps = 30, const IFormat & format = IFormat::BGRA_UINT8 );

			virtual void startCapture() = 0;
			virtual void stopCapture() = 0;

		private:
			static std::vector<CameraInfo> _camInfos;
	};

	inline size_t Camera::count()
	{
		return _camInfos.size();
	}

	inline const CameraInfo & Camera::info( size_t index )
	{
		if( index > _camInfos.size() ){
			throw CVTException( "Could not return CameraInfo: Index out of bounds!" );
		}

		return _camInfos[ index ];
	}
}

#endif
