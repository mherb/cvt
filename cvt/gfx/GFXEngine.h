#ifndef CVT_GFXENGINE_H
#define CVT_GFXENGINE_H

#include <cvt/geom/Rect.h>
#include <cvt/gfx/Color.h>

namespace cvt {
	enum WindingRule = { WINDING_EVEN_ODD, WINDING_NONZERO };
	enum StrokeStyle = { STROKE_MITER, STROKE_ROUND, STROKE_BEVEL };

	class GFXEngine
	{
		public:
			virtual void begin();
			virtual void end();
			virtual void drawLines( const Vector2f* pts, size_t n, float width, const Color& c ) const = 0;
			virtual void fillRect( const Recti& rect, const Color& c ) const = 0;
			virtual void drawRect( const Recti& rect, float width, const Color& c ) const= 0;
			virtual void fillRoundRect( const Recti& rect, float r, const Color& c ) const = 0;
			virtual void drawRoundRect( const Recti& rect, float width, float r const Color& c ) const= 0;
			virtual void drawEllipse( const Vector2f& center, float rx, float ry, float width, const Color& c ) const = 0;
			virtual void fillEllipse( const Vector2f& center, float rx, float ry, const Color& c ) const = 0;
			virtual void strokePath( const Path& path, float width, const Color& c, StrokeStyle style ) const = 0;
			virtual void fillPath( const Path& path, const Color& c, WindingRule wrule ) const = 0;
			virtual void drawText( int x, int y, const char* text, const Color& c ) const = 0;
			virtual void drawImage( int x, int y, const Image& img, float alpha ) const = 0;
			virtual void drawImage( const Recti& rect, const Image& img, float alpha ) const = 0;
	};
}

#endif
