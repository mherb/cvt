#ifndef CVT_CAMERAINFO_H
#define CVT_CAMERAINFO_H

#include <string>
#include <vector>
#include <fstream>
#include <cvt/gfx/Image.h>
#include <cvt/io/CameraMode.h>
#include <cvt/io/CameraModeSet.h>

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

			CameraType type() const;
			void setType( CameraType type );

			size_t index() const;
			void setIndex( size_t index );

			const CameraMode & mode( size_t index ) const;
			CameraMode bestMatchingMode( const IFormat & format, size_t width, size_t height, size_t fps ) const;
			void addMode( const CameraMode & m );
			size_t numModes() const;
			const CameraModeSet & modeSet() const;

		private:
			std::string _name;
			CameraType _type;
			size_t _index;
			CameraModeSet _modeSet;
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

	inline CameraType CameraInfo::type() const
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

	inline const CameraMode & CameraInfo::mode( size_t index ) const
	{
		return _modeSet.mode( index );
	}


	inline CameraMode CameraInfo::bestMatchingMode( const IFormat & format, size_t width, size_t height, size_t fps ) const
	{
		return _modeSet.bestMatching( format, width, height, fps );
	}

	inline void CameraInfo::addMode( const CameraMode & m )
	{
		_modeSet.add( m );
	}

	inline size_t CameraInfo::numModes() const
	{
		return _modeSet.size();
	}

	inline const CameraModeSet & CameraInfo::modeSet() const
	{
		return _modeSet;
	}

	std::ostream& operator<<( std::ostream &out, const CameraInfo &info );
}

#endif
