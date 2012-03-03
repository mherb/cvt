#ifndef CVT_COMPOSITING_SURFACE_H
#define CVT_COMPOSITING_SURFACE_H

#include <cvt/gfx/Image.h>
#include <cvt/gfx/ifilter/ITransform.h>

namespace cvt {
	class CompositingSurface
	{
		public:
			CompositingSurface() :
				_compositedImage( 0 )
			{
				_compositedImage = new Image( 1280, 1024, IFormat::RGBA_UINT8 );
				_compositedImage->fill( Color::BLACK );
			}

			virtual ~CompositingSurface()	
			{
				if( _compositedImage )
					delete _compositedImage;
			}

			/**
			 *	add a new Image to the compositing surface
			 *	the alignment is given by a homography which transforms points 
			 *	from the current to the first image 
			 */
			virtual void addImage( const Image& img, const Matrix3f& homography ) = 0;

			const Image& compositedImage() const { return *_compositedImage; }

		protected:
			Image*	_compositedImage;
	};
}

#endif
