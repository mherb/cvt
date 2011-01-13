#ifndef FLOW_H
#define FLOW_H

#include <cvt/gfx/Image.h>
#include <cvt/util/Exception.h>

namespace cvt {
	namespace Flow {
		float AEE( const Image& flow, const Image& gt );
		float AAE( const Image& flow, const Image& gt );
		void colorCode( Image& dst, Image& flow );
	}
}

#endif
