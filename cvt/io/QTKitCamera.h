#ifndef CVT_QTKITCAMERA_H
#define CVT_QTKITCAMERA_H

#include <cvt/io/Camera.h>
#include <cvt/gfx/Image.h>

namespace cvt {

	class QTKitCameraInterface;
	
	class QTKitCamera : public Camera
	{
		public:
			QTKitCamera( size_t camIndex = 0, 
						 size_t width=640, 
						 size_t height=480, 
						 size_t fps = 30,
						 const IFormat & format = IFormat::BGRA_UINT8 );
		
			virtual ~QTKitCamera();
		
			size_t width() const;
			size_t height() const;
			void nextFrame();
			const Image & frame() const;
			void startCapture();
			void stopCapture();	
		
			const IFormat & format() const;
		
			static size_t count();		
			static void cameraInfo( size_t index, CameraInfo & info );
		
		private:
			QTKitCameraInterface * _device;
			Image _frame;
	};
	
	inline size_t QTKitCamera::width() const
	{
		return _frame.width();
	}
	
	inline size_t QTKitCamera::height() const
	{
		return _frame.height();
	}
	
	inline const Image & QTKitCamera::frame() const
	{
		return _frame;
	}
		
	inline const IFormat & QTKitCamera::format() const
	{
		return _frame.format();
	}
	
}

#endif