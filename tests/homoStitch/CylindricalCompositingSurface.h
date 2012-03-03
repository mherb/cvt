#ifndef CVT_CYLINDRICAL_COMPOSITING_SURFACE_H
#define CVT_CYLINDRICAL_COMPOSITING_SURFACE_H

#include "CompositingSurface.h"

namespace cvt
{
	class CylindricalCompositingSurface : public CompositingSurface
	{
		public:
			CylindricalCompositingSurface( float radius, float focalLength );
			~CylindricalCompositingSurface();

			void addImage( const Image& img, const Matrix3f& homography );

		private:
			struct CylindricalWarp : public Function<Vector2f, Vector2f>
			{
				float		radius;
				float		focal;
				float		cx;
				float		cy;
				size_t		wc;
				size_t		hc;
				Matrix3f	homography;	

				Vector2f operator()( const Vector2f& v ) const;
			};

			CylindricalWarp		_warpFunc;
	};
}

#endif
