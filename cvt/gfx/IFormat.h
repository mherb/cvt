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

		IFORMAT_BAYER_RGGB_UINT8
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
		bool operator==( const IFormat & other ) const;
		bool operator!=( const IFormat & other ) const;
		void operator=( const IFormat & other );

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

		static const IFormat & uint8Equivalent( const IFormat & format );
		static const IFormat & floatEquivalent( const IFormat & format );
	};

	inline bool IFormat::operator==( const IFormat & other ) const
	{
		return ( other.formatID == formatID );
	}

	inline bool IFormat::operator!=( const IFormat & other ) const
	{
		return ( other.formatID != formatID );
	}

	inline void IFormat::operator=( const IFormat & other )
	{
		channels = other.channels;
		bpc = other.bpc;
		bpp = other.bpp;
		formatID = other.formatID;
		type = other.type;
	}

	inline const IFormat & IFormat::uint8Equivalent( const IFormat & format )
	{
		switch ( format.formatID ) {
			case IFORMAT_GRAY_UINT8:
			case IFORMAT_GRAY_UINT16:
			case IFORMAT_GRAY_INT16:
			case IFORMAT_GRAY_FLOAT:
				return IFormat::GRAY_UINT8;
				break;
			case IFORMAT_GRAYALPHA_UINT8:
			case IFORMAT_GRAYALPHA_UINT16:
			case IFORMAT_GRAYALPHA_INT16:
			case IFORMAT_GRAYALPHA_FLOAT:
				return IFormat::GRAYALPHA_UINT8;
				break;
			case IFORMAT_RGBA_UINT8:
			case IFORMAT_RGBA_UINT16:
			case IFORMAT_RGBA_INT16:
			case IFORMAT_RGBA_FLOAT:
				return IFormat::RGBA_UINT8;
				break;
			case IFORMAT_BGRA_UINT8:
			case IFORMAT_BGRA_UINT16:
			case IFORMAT_BGRA_INT16:
			case IFORMAT_BGRA_FLOAT:
				return IFormat::BGRA_UINT8;
				break;
			case IFORMAT_BAYER_RGGB_UINT8:
				return IFormat::BAYER_RGGB_UINT8;
				break;
			default:
				throw CVTException( "NO UINT8 equivalent for requested FORMAT" );
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
				break;
			case IFORMAT_GRAYALPHA_UINT8:
			case IFORMAT_GRAYALPHA_UINT16:
			case IFORMAT_GRAYALPHA_INT16:
			case IFORMAT_GRAYALPHA_FLOAT:
				return IFormat::GRAYALPHA_FLOAT;
				break;
			case IFORMAT_RGBA_UINT8:
			case IFORMAT_RGBA_UINT16:
			case IFORMAT_RGBA_INT16:
			case IFORMAT_RGBA_FLOAT:
				return IFormat::RGBA_FLOAT;
				break;
			case IFORMAT_BGRA_UINT8:
			case IFORMAT_BGRA_UINT16:
			case IFORMAT_BGRA_INT16:
			case IFORMAT_BGRA_FLOAT:
				return IFormat::BGRA_FLOAT;
				break;
			default:
				throw CVTException( "NO UINT8 equivalent for requested FORMAT" );
				break;
		}
	}

	std::ostream& operator<<(std::ostream &out, const IFormat &f);

}

#endif
