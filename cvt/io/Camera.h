/*
			CVT - Computer Vision Tools Library

 	 Copyright (c) 2012, Philipp Heise, Sebastian Klose

 	THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 	KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 	IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 	PARTICULAR PURPOSE.
 */
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
			virtual ~Camera();
			virtual void startCapture() = 0;
			virtual void stopCapture() = 0;
			virtual const String& identifier() const = 0;

			/* number of available cameras */
			static size_t count();
			static const CameraInfo & info( size_t index );
			static void updateInfo();

			/* will create camera with index and closest possible parameters */
			static Camera* get( size_t index, size_t width = 640, size_t height = 480,
							    size_t fps = 60, const IFormat & format = IFormat::BGRA_UINT8 );

		private:
			Camera( const Camera & other );
			static std::vector<CameraInfo> _camInfos;
		protected:
			Camera();
	};

	inline size_t Camera::count()
	{
		return _camInfos.size();
	}

	inline const CameraInfo& Camera::info( size_t index )
	{
		if( index >= _camInfos.size() ){
			throw CVTException( "Could not return CameraInfo: Index out of bounds!" );
		}

		return _camInfos[ index ];
	}
}

#endif
