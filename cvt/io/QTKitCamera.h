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
		
		virtual size_t width();
		virtual size_t height();
		virtual const Image & frame() const ;
		virtual void nextFrame();
		virtual const IOrder order() const;
		virtual const IType type() const;
	
		static size_t count();
		static void nameForIndex( size_t index, std::string & name );
		
	private:
		QTKitCameraInterface * _device;
		Image _frame;
};
	
	inline size_t QTKitCamera::width()
	{
		return _frame.width();
	}
	
	inline size_t QTKitCamera::height()
	{
		return _frame.height();
	}
	
	inline const Image & QTKitCamera::frame() const
	{
		return _frame;
	}
		
	inline const IOrder QTKitCamera::order() const
	{
		return _frame.order();
	}
	
	inline const IType QTKitCamera::type() const
	{
		return _frame.type();
	}
	
}

#endif