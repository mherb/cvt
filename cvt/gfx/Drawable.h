#ifndef CVT_DRAWABLE_H
#define CVT_DRAWABLE_H

#include <cvg/gfx/GFXEngine.h>

namespace cvt {
	class Drawable
	{
		public:
			virtual GFXEngine* GFXEngine() = 0;
	};
}

#endif
