#ifndef CVT_QTKITCAMERA_H
#define CVT_QTKITCAMERA_H

#include <cvt/io/Camera.h>
#include <cvt/gfx/Image.h>

namespace cvt {

	class QTKitCameraInterface;
	
	class QTKitCamera : public Camera
	{
		public:
			QTKitCamera(size_t camIndex = 0, 
						size_t width=640, 
						size_t height=480, 
						size_t fps = 30, 
						IOrder order = IOrder::BGRA,
						IType type = IType::UBYTE);
		
			virtual ~QTKitCamera();
		
			size_t width() const;
			size_t height() const;
			void nextFrame();
			const Image & frame() const;
			void startCapture();
			void stopCapture();	
		
			IOrder order() const;
			IType type() const;
		
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
		
	inline IOrder QTKitCamera::order() const
	{
		return _frame.order();
	}
	
	inline IType QTKitCamera::type() const
	{
		return _frame.type();
	}
	
}

#endif