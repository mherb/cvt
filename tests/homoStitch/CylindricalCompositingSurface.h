#ifndef CVT_CYLINDRICAL_COMPOSITING_SURFACE_H
#define CVT_CYLINDRICAL_COMPOSITING_SURFACE_H

#include "CompositingSurface.h"

namespace cvt
{
	class CylindricalCompositingSurface : public CompositingSurface
	{
		public:
			CylindricalCompositingSurface( float radius );
			~CylindricalCompositingSurface();

			void addImage( const Image& img, const Matrix3f& intrinsics, const Matrix3f& rotation );

		private:
			struct CylindricalWarp : public Function<Vector2f, Vector2f>
			{
				float		radius;
				Matrix3f	rotation;	
				Matrix3f	intrinsics;	
				size_t		compWidth;	
				size_t		compHeight;	

				Vector2f operator()( const Vector2f& v ) const;
			};

			CylindricalWarp		_warpFunc;

	};
}

#endif
