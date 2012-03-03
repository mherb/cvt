#ifndef CVT_PLANE_COMPOSITING_SURFACE_H
#define CVT_PLANE_COMPOSITING_SURFACE_H

#include "CompositingSurface.h"

namespace cvt
{
	class PlaneCompositingSurface : public CompositingSurface
	{
		public:
			PlaneCompositingSurface();
			~PlaneCompositingSurface();
			
			void addImage( const Image& img, const Matrix3f& intrinsics, const Matrix3f& rot );

		private:
			bool		_first;
			Matrix3f	_offsetTransform;
			Matrix3f	_intrinsics0Inv;

	};
}

#endif
