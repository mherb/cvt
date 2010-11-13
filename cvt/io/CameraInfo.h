#ifndef CVT_CAMERAINFO_H
#define CVT_CAMERAINFO_H

#include <string>
#include <fstream>
#include <cvt/gfx/Image.h>

namespace cvt {
	
	enum CameraType
	{
		CAMERATYPE_V4L2,
		CAMERATYPE_QTKIT,
		CAMERATYPE_DC1394,
		CAMERATYPE_UEYE
	};
	
	static const std::string CamTypeString[] =
	{
		"V4L2", "QTKIT", "DC1394", "UEYE"
	};
	
	class CameraInfo
	{	
		friend class Camera;
			
		public:
			const std::string & name() const;			
			const CameraType type() const;
			size_t index() const;
			size_t maxFPS() const;
			size_t maxWidth() const;
			size_t maxHeight() const;
			IOrder imageOrder() const;
			IType imageType() const;
					
		private:		
			CameraInfo();
		
			std::string _name;
			CameraType _type;
			size_t _index;
			size_t _maxFPS;
			size_t _maxWidth;
			size_t _maxHeight;
			IOrder _imageOrder;
			IType _imageType;
	};
	
	inline CameraInfo::CameraInfo()
	{		
	}
	
	inline const std::string & CameraInfo::name() const
	{
		return _name;
	}
	
	inline const CameraType CameraInfo::type() const
	{
		return _type;
	}
	
	inline size_t CameraInfo::index() const
	{
		return _index;		
	}
	
	inline size_t CameraInfo::maxFPS() const
	{
		return _maxFPS;
	}
	
	inline size_t CameraInfo::maxWidth() const
	{
		return _maxWidth;
	}
	
	inline size_t CameraInfo::maxHeight() const
	{
		return _maxHeight;
	}
	
	inline IOrder CameraInfo::imageOrder() const
	{
		return _imageOrder;
	}
	
	inline IType CameraInfo::imageType() const
	{
		return _imageType;
	}
}

#endif