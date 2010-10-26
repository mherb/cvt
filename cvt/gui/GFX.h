#ifndef CVTGFX_H
#define CVTGFX_H

#include <cvt/util/Rect.h>
#include <cvt/gfx/Color.h>
#include <cvt/gfx/Image.h>

namespace cvt {
	class GFX {
		public:
			GFX();
			//GFX( const GFX& g );
			virtual ~GFX();

			void setDefault();
			void setColor( const Color& c ) { color = c; };
			Color& getColor() { return color; };

			virtual void fillRect( const Recti& rect ) = 0;
			virtual void fillRect( int x, int y, int width, int height ) = 0;
			virtual void drawText( int x, int y, const char* text ) = 0;
			virtual void drawImage( int x, int y, const Image& img ) = 0;

		protected:
			Color color;
	};
}

#endif
