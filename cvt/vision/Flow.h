#ifndef FLOW_H
#define FLOW_H

#include "gfx/Image.h"
#include "util/Exception.h"

namespace cvt {
	namespace Flow {
		float AEE( const Image& flow, const Image& gt );
		float AAE( const Image& flow, const Image& gt );
		void colorCode( Image& dst, Image& flow );
	}
}

#endif
