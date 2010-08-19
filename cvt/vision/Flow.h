#ifndef FLOW_H
#define FLOW_H

#include "gfx/Image.h"
#include "util/Exception.h"

namespace cvt {
	namespace Flow {
		float AEE( Image& flow, Image& gt );
		float AAE( Image& flow, Image& gt );
		void colorCode( Image& dst, Image& flow );
	}
}

#endif
