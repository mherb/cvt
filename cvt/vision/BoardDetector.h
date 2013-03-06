#ifndef CVT_BOARDDETECTOR_H
#define CVT_BOARDDETECTOR_H

#include <cvt/gfx/Image.h>
#include <cvt/gfx/IComponents.h>
#include <cvt/gfx/ifilter/Canny.h>
#include <cvt/geom/Ellipse.h>
#include <cvt/geom/Line2D.h>
#include <cvt/geom/PointSet.h>

namespace cvt{

	class BoardDetector {
		public:
			BoardDetector() {}
			~BoardDetector() {}

			bool detectACirclePattern( PointSet2f& h, const Image& im, size_t dotCount1, size_t dotCount2 );

		private:
	};
}

#endif
