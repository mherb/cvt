#include "gfx/ImageTypes.h"
#include <stdint.h>

namespace cvt {
	const IOrder IOrder::GRAY		 = { ICHANNELORDER_GRAY, 1 };
	const IOrder IOrder::GRAYALPHA   = { ICHANNELORDER_GRAYALPHA, 2 };
	const IOrder IOrder::RGBA		 = { ICHANNELORDER_RGBA, 4 };
	const IOrder IOrder::BGRA		 = { ICHANNELORDER_BGRA, 4 };

	const IType IType::UBYTE		= { ICHANNELTYPE_UBYTE, sizeof( uint8_t ) };
	const IType IType::FLOAT		= { ICHANNELTYPE_FLOAT, sizeof( float ) };
}


