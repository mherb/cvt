#ifndef CVT_IFORMAT_H
#define CVT_IFORMAT_H

#include <stdlib.h>
#include <cvt/util/Exception.h>
#include <fstream>

namespace cvt
{
	enum IFormatID
	{
		IFORMAT_GRAY_UINT8 = 1,
		IFORMAT_GRAY_UINT16,
		IFORMAT_GRAY_INT16,
		IFORMAT_GRAY_FLOAT,

		IFORMAT_GRAYALPHA_UINT8,
		IFORMAT_GRAYALPHA_UINT16,
		IFORMAT_GRAYALPHA_INT16,
		IFORMAT_GRAYALPHA_FLOAT,

		IFORMAT_RGBA_UINT8,
		IFORMAT_RGBA_UINT16,
		IFORMAT_RGBA_INT16,
		IFORMAT_RGBA_FLOAT,

		IFORMAT_BGRA_UINT8,
		IFORMAT_BGRA_UINT16,
		IFORMAT_BGRA_INT16,
		IFORMAT_BGRA_FLOAT,

		IFORMAT_BAYER_RGGB_UINT8,
		IFORMAT_YUYV_UINT8,
		IFORMAT_UYVY_UINT8
	};

	enum IFormatType
	{
		IFORMAT_TYPE_OTHER = 0,
		IFORMAT_TYPE_UINT8,
		IFORMAT_TYPE_UINT16,
		IFORMAT_TYPE_INT16,
		IFORMAT_TYPE_FLOAT
	};

	struct IFormat
	{
		IFormat( const IFormat & f );
		bool operator==( const IFormat & other ) const;
		bool operator!=( const IFormat & other ) const;

		size_t channels;
		size_t bpc;
		size_t bpp;
		IFormatID formatID;
		IFormatType type;

		static const IFormat GRAY_UINT8;
		static const IFormat GRAY_UINT16;
		static const IFormat GRAY_INT16;
		static const IFormat GRAY_FLOAT;
		static const IFormat GRAYALPHA_UINT8;
		static const IFormat GRAYALPHA_UINT16;
		static const IFormat GRAYALPHA_INT16;
		static const IFormat GRAYALPHA_FLOAT;
		static const IFormat RGBA_UINT8;
		static const IFormat RGBA_UINT16;
		static const IFormat RGBA_INT16;
		static const IFormat RGBA_FLOAT;
		static const IFormat BGRA_UINT8;
		static const IFormat BGRA_UINT16;
		static const IFormat BGRA_INT16;
		static const IFormat BGRA_FLOAT;
		static const IFormat BAYER_RGGB_UINT8;
		static const IFormat YUYV_UINT8;
		static const IFormat UYVY_UINT8;

		static const IFormat & uint8Equivalent( const IFormat & format );
		static const IFormat & int16Equivalent( const IFormat & format );
		static const IFormat & floatEquivalent( const IFormat & format );

		private:
			IFormat( size_t c, size_t bpc, size_t bpp, IFormatID formatID, IFormatType type );
	};

	inline bool IFormat::operator==( const IFormat & other ) const
	{
		return ( other.formatID == formatID );
	}

	inline bool IFormat::operator!=( const IFormat & other ) const
	{
		return ( other.formatID != formatID );
	}

	inline const IFormat & IFormat::uint8Equivalent( const IFormat & format )
	{
		switch ( format.formatID ) {
			case IFORMAT_GRAY_UINT8:
			case IFORMAT_GRAY_UINT16:
			case IFORMAT_GRAY_INT16:
			case IFORMAT_GRAY_FLOAT:
				return IFormat::GRAY_UINT8;
			case IFORMAT_GRAYALPHA_UINT8:
			case IFORMAT_GRAYALPHA_UINT16:
			case IFORMAT_GRAYALPHA_INT16:
			case IFORMAT_GRAYALPHA_FLOAT:
				return IFormat::GRAYALPHA_UINT8;
			case IFORMAT_RGBA_UINT8:
			case IFORMAT_RGBA_UINT16:
			case IFORMAT_RGBA_INT16:
			case IFORMAT_RGBA_FLOAT:
				return IFormat::RGBA_UINT8;
			case IFORMAT_BGRA_UINT8:
			case IFORMAT_BGRA_UINT16:
			case IFORMAT_BGRA_INT16:
			case IFORMAT_BGRA_FLOAT:
				return IFormat::BGRA_UINT8;
			case IFORMAT_BAYER_RGGB_UINT8:
				return IFormat::BAYER_RGGB_UINT8;
			case IFORMAT_YUYV_UINT8:
				return IFormat::YUYV_UINT8;
			case IFORMAT_UYVY_UINT8:
				return IFormat::UYVY_UINT8;
			default:
				throw CVTException( "NO UINT8 equivalent for requested FORMAT" );
		}
	}

	inline const IFormat & IFormat::int16Equivalent( const IFormat & format )
	{
		switch ( format.formatID ) {
			case IFORMAT_GRAY_UINT8:
			case IFORMAT_GRAY_UINT16:
			case IFORMAT_GRAY_INT16:
			case IFORMAT_GRAY_FLOAT:
				return IFormat::GRAY_INT16;
			case IFORMAT_GRAYALPHA_UINT8:
			case IFORMAT_GRAYALPHA_UINT16:
			case IFORMAT_GRAYALPHA_INT16:
			case IFORMAT_GRAYALPHA_FLOAT:
				return IFormat::GRAYALPHA_INT16;
			case IFORMAT_RGBA_UINT8:
			case IFORMAT_RGBA_UINT16:
			case IFORMAT_RGBA_INT16:
			case IFORMAT_RGBA_FLOAT:
				return IFormat::RGBA_INT16;
			case IFORMAT_BGRA_UINT8:
			case IFORMAT_BGRA_UINT16:
			case IFORMAT_BGRA_INT16:
			case IFORMAT_BGRA_FLOAT:
				return IFormat::BGRA_INT16;
			default:
				throw CVTException( "NO INT16 equivalent for requested FORMAT" );
				break;
		}
	}
	inline const IFormat & IFormat::floatEquivalent( const IFormat & format )
	{
		switch ( format.formatID ) {
			case IFORMAT_GRAY_UINT8:
			case IFORMAT_GRAY_UINT16:
			case IFORMAT_GRAY_INT16:
			case IFORMAT_GRAY_FLOAT:
				return IFormat::GRAY_FLOAT;
			case IFORMAT_GRAYALPHA_UINT8:
			case IFORMAT_GRAYALPHA_UINT16:
			case IFORMAT_GRAYALPHA_INT16:
			case IFORMAT_GRAYALPHA_FLOAT:
				return IFormat::GRAYALPHA_FLOAT;
			case IFORMAT_RGBA_UINT8:
			case IFORMAT_RGBA_UINT16:
			case IFORMAT_RGBA_INT16:
			case IFORMAT_RGBA_FLOAT:
				return IFormat::RGBA_FLOAT;
			case IFORMAT_BGRA_UINT8:
			case IFORMAT_BGRA_UINT16:
			case IFORMAT_BGRA_INT16:
			case IFORMAT_BGRA_FLOAT:
				return IFormat::BGRA_FLOAT;
			default:
				throw CVTException( "NO UINT8 equivalent for requested FORMAT" );
				break;
		}
	}

	std::ostream& operator<<(std::ostream &out, const IFormat &f);

}

#endif
