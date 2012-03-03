#include "CylindricalCompositingSurface.h"

namespace cvt
{
	CylindricalCompositingSurface::CylindricalCompositingSurface( float radius )
	{
		int width = Math::TWO_PI * radius;
		_compositedImage->reallocate( width, 1024, IFormat::RGBA_UINT8 );

		_warpFunc.rotation.setIdentity();
		_warpFunc.intrinsics.setIdentity();
		_warpFunc.radius = radius;
		_warpFunc.compWidth = _compositedImage->width();
		_warpFunc.compHeight = _compositedImage->height();
	}

	CylindricalCompositingSurface::~CylindricalCompositingSurface()
	{
	}
			
	void CylindricalCompositingSurface::addImage( const Image& img, const Matrix3f& intrinsics, const Matrix3f& rotation )
	{		
		_warpFunc.rotation   = rotation;
		_warpFunc.intrinsics = intrinsics;

		ITransform::apply( *_compositedImage, img, _warpFunc, _compositedImage->width(), _compositedImage->height() );
	}

	Vector2f CylindricalCompositingSurface::CylindricalWarp::operator()( const Vector2f& cyl ) const
	{
		// this is the inverse lookup:
		Vector3f cyl3;
		float angle = Math::TWO_PI * cyl.x / ( float )compWidth - Math::PI;
		cyl3.x = sin( angle );
		float s = ( float )compWidth / Math::TWO_PI;
		cyl3.y = ( cyl.y - 0.5f * compHeight ) / s;
		cyl3.z = cos( angle );

		if( cyl.y == 100 ){
			std::cout << "Angle: " << Math::rad2Deg( angle ) << std::endl;
		}

		// project the vector to the image plane
		Vector3f image;
		image = intrinsics * rotation.transpose() * cyl3;

		Vector2f ret( -1.0f, -1.0f );
		if( image.z > 0.0f ){
			ret.x = image.x / image.z;
			ret.y = image.y / image.z;
		}

		return ret;
	}
}
