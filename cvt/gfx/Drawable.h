#ifndef CVT_DRAWABLE_H
#define CVT_DRAWABLE_H

namespace cvt {
	class GFXEngine;

	class Drawable
	{
		public:
			virtual ~Drawable() {};
			virtual GFXEngine* gfxEngine() = 0;
	};
}

#endif
