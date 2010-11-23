#ifndef CVTGFX_H
#define CVTGFX_H

#include <cvt/util/Rect.h>
#include <cvt/gfx/Color.h>
#include <cvt/gfx/Image.h>

namespace cvt {

	class GFX {
		friend class Widget;
		friend class WidgetImplWinGLX11;

		public:
			GFX();
			//GFX( const GFX& g );
			virtual ~GFX();

			void setDefault();
			void setColor( const Color& c ) { _color = c; };
			Color& color() { return _color; };

			virtual void fillRect( const Recti& rect ) = 0;
			virtual void fillRect( int x, int y, int width, int height ) = 0;
			virtual void fillRoundRect( const Recti& rect, float radius ) = 0;
			virtual void fillRoundRect( int x, int y, int width, int height, float radius ) = 0;
			virtual void drawText( int x, int y, const char* text ) = 0;
			virtual void drawImage( int x, int y, const Image& img ) = 0;
			virtual void drawImage( int x, int y, int width, int height, const Image& img ) = 0;

		private:
			void setViewport( const Recti& r ) { _viewport = r; };
			void viewport( Recti& r ) const { r = _viewport; };
			void setChildrect( const Recti& r ) { _childrect = r; updateState(); };
			void childrect( Recti& r ) const { r = _childrect; };


		protected:
			virtual void updateState() = 0;

			Color _color;
			Recti _viewport;
			Recti _childrect;
	};
}

#endif
