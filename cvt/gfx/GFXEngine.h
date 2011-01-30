#ifndef CVT_GFXENGINE_H
#define CVT_GFXENGINE_H

#include <cvt/geom/Rect.h>
#include <cvt/math/Vector.h>
#include <cvt/gfx/Color.h>
#include <cvt/gfx/Path.h>

namespace cvt {

	class GFXEngine
	{
		public:
			enum PolygonFillRule { WINDING_EVEN_ODD, WINDING_NONZERO };
			enum StrokeStyle { STROKE_MITER, STROKE_ROUND, STROKE_BEVEL };

			virtual ~GFXEngine() {};
			virtual void begin() = 0;
			virtual void end() = 0;
			virtual void drawLines( const Vector2f* pts, size_t n, float width, const Color& c ) const = 0;
			virtual void fillRect( const Recti& rect, const Color& c ) const = 0;
			virtual void drawRect( const Recti& rect, float width, const Color& c ) const = 0;
			virtual void fillRoundRect( const Recti& rect, float r, const Color& c ) const = 0;
			virtual void drawRoundRect( const Recti& rect, float width, float r, const Color& c ) const = 0;
			virtual void drawEllipse( const Vector2f& center, float rx, float ry, float width, const Color& c ) const = 0;
			virtual void fillEllipse( const Vector2f& center, float rx, float ry, const Color& c ) const = 0;
			virtual void strokePath( const Pathf& path, float width, const Color& c, GFXEngine::StrokeStyle style ) const = 0;
			virtual void fillPath( const Pathf& path, const Color& c, GFXEngine::PolygonFillRule rule ) const = 0;
			virtual void drawText( int x, int y, const char* text, const Color& c ) const = 0;
			virtual void drawImage( int x, int y, const Image& img, float alpha ) const = 0;
			virtual void drawImage( const Recti& rect, const Image& img, float alpha ) const = 0;
	};
}

#endif
