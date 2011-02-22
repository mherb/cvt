#ifndef CVT_GFXENGINE_H
#define CVT_GFXENGINE_H

#include <cvt/geom/Rect.h>
#include <cvt/math/Vector.h>
#include <cvt/gfx/Color.h>
#include <cvt/gfx/Path.h>
#include <cvt/gfx/GFX.h>
#include <cvt/gfx/Font.h>

namespace cvt {

	class GFXEngine
	{
		public:

			virtual ~GFXEngine() {};
			virtual void begin() = 0;
			virtual void end() = 0;
			virtual void drawLines( const Vector2f* pts, size_t n, float width, const Color& c ) = 0;
			virtual void fillRect( const Recti& rect, const Color& c ) = 0;
			virtual void drawRect( const Recti& rect, float width, const Color& c ) = 0;
			virtual void fillRoundRect( const Recti& rect, float r, const Color& c ) = 0;
			virtual void drawRoundRect( const Recti& rect, float width, float r, const Color& c ) = 0;
			virtual void drawEllipse( const Vector2f& center, float rx, float ry, float width, const Color& c ) = 0;
			virtual void fillEllipse( const Vector2f& center, float rx, float ry, const Color& c ) = 0;
			virtual void strokePath( const Pathf& path, float width, const Color& c, GFX::StrokeStyle style ) = 0;
			virtual void fillPath( const Pathf& path, const Color& c, GFX::PolygonFillRule rule ) = 0;
			virtual void drawText( int x, int y, const char* text, const Color& c ) = 0;
			virtual void drawImage( int x, int y, const Image& img, float alpha ) = 0;
			virtual void drawImage( const Recti& rect, const Image& img, float alpha ) = 0;
			virtual void drawIcon( int x, int y, GFX::Icon i, const Color& c ) = 0;
			virtual void drawIcons( const Vector2i* pts, size_t npts, GFX::Icon i, const Color& c ) = 0;
			virtual void drawIcons( const Vector2f* pts, size_t npts, GFX::Icon i, const Color& c ) = 0;
			virtual const Font& font() const = 0;

			/* optional - only used by the Widget in paintChild */
			virtual void setChildrect( const Recti& childrect ) = 0;
			virtual const Recti& childrect() const = 0;

	};
}

#endif
