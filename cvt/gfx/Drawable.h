#ifndef CVT_DRAWABLE_H
#define CVT_DRAWABLE_H

#include <cvt/gfx/GFXEngine.h>

namespace cvt {
	class Drawable
	{
		public:
			virtual ~Drawable() {};
			virtual GFXEngine* gfxEngine() = 0;
	};
}

#endif
