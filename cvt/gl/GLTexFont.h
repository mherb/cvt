#ifndef CVT_GLTEXTFONT_H
#define CVT_GLTEXTFONT_H

#include <cvt/gfx/Font.h>
#include <cvt/gl/GLTexture.h>

namespace cvt {

	/**
	  \ingroup GL
	*/
	class GLTexFont : public Font {
		public:
			GLTexFont();
			void load( const char* path );
			int spriteSize() const;
			const GLTexture& texture() const;
			int size() const;
			int ascent() const;
			int descent() const;
			int offsetX() const;
			int offsetY() const;
			Recti stringBounds( const char* str, size_t n = 0 ) const;
			int advance( size_t glyph ) const;

		private:
			GLTexFont( const GLTexFont& );
			const GLTexFont& operator=( const GLTexFont& );

			int32_t _width, _height;
			int32_t _fontsize;
			int32_t _ascent, _descent;
			int32_t _spritesize;
			int32_t _offx, _offy;
			struct GlyphMetric {
				int32_t width, height;
				int32_t bearingX, bearingY;
				int32_t advance;
			};
			GlyphMetric _metric[ 256 ];
			GLTexture _tex;
	};

	inline int GLTexFont::spriteSize() const
	{
		return _spritesize;
	}

	inline const GLTexture& GLTexFont::texture() const
	{
		return _tex;
	}

	inline int GLTexFont::size() const
	{
		return _fontsize;
	}

	inline int GLTexFont::ascent() const
	{
		return _ascent;
	}

	inline int GLTexFont::descent() const
	{
		return _descent;
	}

	inline int GLTexFont::offsetX() const
	{
		return _offx;
	}

	inline int GLTexFont::offsetY() const
	{
		return _offy;
	}



	inline Recti GLTexFont::stringBounds( const char* _str, size_t n ) const
	{
		uint8_t* str = ( uint8_t* ) _str;
		Recti rect;
		int x, h;

		h = ascent() - descent();

		if( str ) {
			if( n && *str ) {
				rect.set( _metric[ *str ].bearingX, 0, _metric[ *str ].width, h );
				x = _metric[ *str ].bearingX + _metric[ *str ].advance;
				str++;
				n--;
				while( *str && n-- ) {
					rect.join( x + _metric[ *str ].bearingX, 0, _metric[ *str ].width, h );
					x += _metric[ *str ].advance;
					str++;
				}
			} else {
				if( *str ) {
					rect.set( _metric[ *str ].bearingX, 0, _metric[ *str ].width, h );
					x = _metric[ *str ].bearingX + _metric[ *str ].advance;
					str++;
					while( *str ) {
						rect.join( x + _metric[ *str ].bearingX, 0, _metric[ *str ].width, h );
						x += _metric[ *str ].advance;
						str++;
					}
				}
			}
		}
		return rect;
	}


	inline int GLTexFont::advance( size_t glyph ) const
	{
		if( glyph > 0xff )
			return 0;
		return _metric[ glyph ].advance;
	}
}

#endif
