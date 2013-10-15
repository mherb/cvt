#ifndef CVTGFX_H
#define CVTGFX_H

#include <cvt/geom/Rect.h>
#include <cvt/gfx/Color.h>
#include <cvt/gfx/Image.h>
#include <cvt/math/Vector.h>
#include <cvt/gfx/Drawable.h>
#include <cvt/gfx/Font.h>
#include <cvt/gfx/Alignment.h>
#include <cvt/gfx/Path.h>

namespace cvt {
	class GFXEngine;

	class GFX {
		friend class Widget;
		friend class WidgetImplWinGLX11;
        friend class GLView;

		public:
		enum PolygonFillRule { WINDING_EVEN_ODD, WINDING_NONZERO };
		enum StrokeStyle { STROKE_MITER, STROKE_ROUND, STROKE_BEVEL };
		enum Icon { ICON_CIRCLECROSS = 0,
					ICON_RECTCROSS,
					ICON_CIRCLERIGHT,
					ICON_CIRCLELEFT,
					ICON_RECT,
					ICON_CIRCLE,
					ICON_CIRCLEUP,
					ICON_CIRCLEDOWN,
					ICON_CORNER,
					ICON_CROSS,
					ICON_CIRCLEEMPTY,
					ICON_TINY_CROSS	};

			GFX();
			GFX( const GFX& g );
			GFX( GFXEngine* engine );
			GFX( Drawable* drawable );
			~GFX();

			void begin();
			void begin( GFXEngine* engine );
			void begin( Drawable* engine );
			void end();

			void setDefault();

			void setColor( const Color& c );
			Color& color();
			void setLineWidth( float w );
			float lineWidth();

			void drawLine( int x1, int y1, int x2, int y2 );
			void drawLine( const Vector2f& pt1, const Vector2f& pt2 );
			void drawLines( const Vector2f* pts, size_t n );

			void fillRect( const Recti& rect );
			void fillRect( int x, int y, int width, int height );

			void drawRect( const Recti& rect );

			void fillRoundRect( const Recti& rect, float radius );
			void fillRoundRect( int x, int y, int width, int height, float radius );

			void fillPath( const Pathf& path, GFX::PolygonFillRule rule = GFX::WINDING_NONZERO );
			void strokePath( const Pathf& path );

			const Font& font( ) const;
			void drawText( int x, int y, const char* text );
			void drawText( const Recti& rect, Alignment alignment, const char* text );
			void drawText( int x, int y, int width, int height, Alignment alignment, const char* text );

			void drawImage( int x, int y, const Image& img );
			void drawImage( int x, int y, int width, int height, const Image& img );

			void drawIcon( int x, int y, Icon i );
			void drawIcons( const Vector2i* pts, size_t npts, Icon i );
			void drawIcons( const Vector2f* pts, size_t npts, Icon i );

			bool supportsGL() const;
			void beginGL();
			void endGL();

			void setTranslation( const Vector2i& translation );
			void setTranslation( int tx, int ty );

			const Vector2i& translation() const;

		private:
			void setClipRect( const Recti& r );
			const Recti& clipRect() const;

			/* setDefault needs to be called after changing the global translation */
			void setTranslationGlobal( const Vector2i& translation );
			void getTranslationGlobal( Vector2i& translation ) const;

			GFXEngine* _engine;
			bool	   _active;
			Color	   _color;
			float	   _linewidth;
			Vector2i   _translation;
			Vector2i   _translationGlobal;
	};

	inline void GFX::setColor( const Color& c )
	{
		_color = c;
	}

	inline Color& GFX::color()
	{
		return _color;
	}

	inline void GFX::setLineWidth( float w )
	{
		_linewidth = w;
	}

	inline float GFX::lineWidth()
	{
		return _linewidth;
	}

	inline void GFX::drawText( int x, int y, int width, int height, Alignment alignment, const char* text )
	{
		drawText( Recti( x, y, width, height ), alignment, text );
	}
}

#endif
