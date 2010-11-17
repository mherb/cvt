#include <cvt/gfx/IFormat.h>
#include <stdint.h>

namespace cvt {

#define FORMATDESC( c,t, id ) { c, sizeof( t), c*sizeof(t), id }

	const IFormat IFormat::GRAY_UINT8			= FORMATDESC( 1, uint8_t	, IFORMAT_GRAY_UINT8 );
	const IFormat IFormat::GRAY_UINT16			= FORMATDESC( 1, uint16_t	, IFORMAT_GRAY_UINT16 );
	const IFormat IFormat::GRAY_INT16			= FORMATDESC( 1, int16_t	, IFORMAT_GRAY_INT16 );
	const IFormat IFormat::GRAY_FLOAT			= FORMATDESC( 1, float		, IFORMAT_GRAY_FLOAT );
	const IFormat IFormat::GRAYALPHA_UINT8		= FORMATDESC( 2, uint8_t	, IFORMAT_GRAYALPHA_UINT8 );
	const IFormat IFormat::GRAYALPHA_UINT16		= FORMATDESC( 2, uint16_t	, IFORMAT_GRAYALPHA_UINT16);
	const IFormat IFormat::GRAYALPHA_INT16		= FORMATDESC( 2, int16_t	, IFORMAT_GRAYALPHA_INT16 );
	const IFormat IFormat::GRAYALPHA_FLOAT		= FORMATDESC( 2, float		, IFORMAT_GRAYALPHA_FLOAT );
	const IFormat IFormat::RGBA_UINT8			= FORMATDESC( 4, uint8_t	, IFORMAT_RGBA_UINT8 );
	const IFormat IFormat::RGBA_UINT16			= FORMATDESC( 4, uint16_t	, IFORMAT_RGBA_UINT16);
	const IFormat IFormat::RGBA_INT16			= FORMATDESC( 4, int16_t	, IFORMAT_RGBA_INT16 );
	const IFormat IFormat::RGBA_FLOAT			= FORMATDESC( 4, float		, IFORMAT_RGBA_FLOAT );
	const IFormat IFormat::BGRA_UINT8			= FORMATDESC( 4, uint8_t	, IFORMAT_BGRA_UINT8 );
	const IFormat IFormat::BGRA_UINT16			= FORMATDESC( 4, uint16_t	, IFORMAT_BGRA_UINT16 );
	const IFormat IFormat::BGRA_INT16			= FORMATDESC( 4, int16_t	, IFORMAT_BGRA_INT16 );
	const IFormat IFormat::BGRA_FLOAT			= FORMATDESC( 4, float		, IFORMAT_BGRA_FLOAT );
	const IFormat IFormat::BAYER_RGGB_UINT8		= FORMATDESC( 1, uint8_t	, IFORMAT_BAYER_RGGB_UINT8 );

#undef FORMATDESC

}

