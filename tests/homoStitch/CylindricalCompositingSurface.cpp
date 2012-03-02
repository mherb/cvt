#include "CylindricalCompositingSurface.h"

namespace cvt
{
	CylindricalCompositingSurface::CylindricalCompositingSurface( float radius, float focal )
	{
		int width = Math::PI * radius;
		//int width = radius;
		//_compositedImage->reallocate( width, 1024, IFormat::RGBA_UINT8 );

		_warpFunc.homography.setIdentity();
		_warpFunc.radius = radius;
		_warpFunc.focal = focal;
		_warpFunc.cx = 0;
		_warpFunc.cy = 0;
		_warpFunc.wc = _compositedImage->width() / 2.0f;
		_warpFunc.hc = _compositedImage->height() / 2.0f;
	}

	CylindricalCompositingSurface::~CylindricalCompositingSurface()
	{
	}
			
	void CylindricalCompositingSurface::addImage( const Image& img, const Matrix3f& homography )
	{		
		_warpFunc.homography = homography;
		_warpFunc.cx = img.width() / 2.0f;
		_warpFunc.cy = img.height() / 2.0f;
		if( !_warpFunc.homography.inverseSelf() ){
			std::cout << "Could not invert homography!" << std::endl;
			return;
		}

		ITransform::apply( *_compositedImage, img, _warpFunc, _compositedImage->width(), _compositedImage->height() );
	}

	Vector2f CylindricalCompositingSurface::CylindricalWarp::operator()( const Vector2f& cyl ) const
	{
		// this is the inverse lookup:
		// so first get from the cylinder to the base plane
		Vector2f base;
		float tangens = tan( ( ( cyl.x - wc ) / radius ) - Math::HALF_PI );
		base.x = focal * tangens + cx;
	    base.y = ( cyl.y - hc ) / radius * focal * Math::sqrt( 1.0f + Math::sqr( tangens ) ) + cy;

		return base;
		// from base to image	
		Vector2f image;
		image = homography * base;
		return image;
	}
}
