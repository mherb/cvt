#ifndef CVT_IFORMAT_H
#define CVT_IFORMAT_H

#include <stdlib.h>

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

	struct IFormat
	{
		size_t channels;
		size_t bpc;
		size_t bpp;
		IFormatID id;

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

	};
}

#endif
