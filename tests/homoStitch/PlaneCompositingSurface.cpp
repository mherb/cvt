#include "PlaneCompositingSurface.h"

namespace cvt
{
	PlaneCompositingSurface::PlaneCompositingSurface()
	{
		_offsetTransform.setIdentity();
		_offsetTransform[ 0 ][ 0 ] = 1.0f;
		_offsetTransform[ 1 ][ 1 ] = 1.0f;
		_offsetTransform[ 0 ][ 2 ] = _compositedImage->width() / 4;
		_offsetTransform[ 1 ][ 2 ] = _compositedImage->height() / 4;
	}

	PlaneCompositingSurface::~PlaneCompositingSurface()
	{
	}

	void PlaneCompositingSurface::addImage( const Image& img, const Matrix3f& hom )
	{
		// first try if the image is still big enough to capture the content
		Vector2f pt( 0, 0 );
		Vector2f p0, p1, p2, p3;

		Matrix3f homAll = _offsetTransform * hom;

		p0 = homAll * pt; 
		pt.x = img.width();
		p1 = homAll * pt; 
		pt.y = img.height();
		p2 = homAll * pt; 
		pt.x = 0.0f; 
		p3 = homAll * pt;

		float leftBorder = 0.0f;
		if( p0.x < leftBorder ) leftBorder = p0.x;
		if( p1.x < leftBorder ) leftBorder = p1.x;
		if( p2.x < leftBorder ) leftBorder = p2.x;
		if( p3.x < leftBorder ) leftBorder = p3.x;
		
		float rightBorder = _compositedImage->width();
		if( p0.x >= rightBorder ) rightBorder = p0.x;
		if( p1.x >= rightBorder ) rightBorder = p1.x;
		if( p2.x >= rightBorder ) rightBorder = p2.x;
		if( p3.x >= rightBorder ) rightBorder = p3.x;

		float topBorder = 0.0f;
		if( p0.y < topBorder ) topBorder = p0.y;
		if( p1.y < topBorder ) topBorder = p1.y;
		if( p2.y < topBorder ) topBorder = p2.y;
		if( p3.y < topBorder ) topBorder = p3.y;
		
		float bottomBorder = _compositedImage->height();
		if( p0.y >= bottomBorder ) rightBorder = p0.y;
		if( p1.y >= bottomBorder ) rightBorder = p1.y;
		if( p2.y >= bottomBorder ) rightBorder = p2.y;
		if( p3.y >= bottomBorder ) rightBorder = p3.y;

		if( leftBorder   <  0.0f || 
		    topBorder    <  0.0f ||
		    rightBorder	 > _compositedImage->width() ||
			bottomBorder > _compositedImage->height() ){

			// we need to resize the stitched view:
			size_t w = _compositedImage->width();
			size_t h = _compositedImage->height();

			size_t l = 0;
			if( leftBorder < 0.0f )
				l = Math::abs( leftBorder ) + 1;
			size_t t = 0; 
			if( topBorder < 0.0f )
				t = Math::abs( topBorder ) + 1;
			
			int r = ( int )rightBorder - ( int )_compositedImage->width();
			if( r > 0 )
				w += r;
			int b = ( int )bottomBorder - ( int )_compositedImage->height();
			if( b > 0 )
				h += b;

			w += l;
			h += t;
			Image* tmp = new Image( w, h, IFormat::RGBA_UINT8 );
			tmp->fill( Color::BLACK );
			
			// copy the previously stitched one here:
			tmp->copyRect( l, t, *_compositedImage, _compositedImage->rect() );
			
			delete _compositedImage;
			_compositedImage = tmp;

			_offsetTransform[ 0 ][ 2 ] += l;
			_offsetTransform[ 1 ][ 2 ] += t;

			homAll = _offsetTransform * hom;
		}

		ITransform::apply( *_compositedImage, img, homAll, _compositedImage->width(), _compositedImage->height() );
	}
}
