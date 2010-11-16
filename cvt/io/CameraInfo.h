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
	
	class CameraInfo
	{	
		public:
			CameraInfo();
			const std::string & name( ) const;			
			void setName( const std::string & name );
			const CameraType type() const;
			void setType( CameraType type );
			size_t index() const;
			void setIndex( size_t index );
			
		private:		
			std::string _name;
			CameraType _type;
			size_t _index;
	};
	
	inline CameraInfo::CameraInfo()
	{		
	}
	
	inline const std::string & CameraInfo::name() const
	{
		return _name;
	}
	
	inline void CameraInfo::setName( const std::string & name )
	{
		_name = name;
	}
	
	inline const CameraType CameraInfo::type() const
	{
		return _type;
	}
	
	inline void CameraInfo::setType( CameraType type )
	{
		_type = type;
	}
	
	inline size_t CameraInfo::index() const
	{
		return _index;		
	}
	
	inline void CameraInfo::setIndex( size_t index )
	{
		_index = index;
	}
}

#endif