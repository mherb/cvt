#ifndef CVT_GLTEXTFONT_H
#define CVT_GLTEXTFONT_H

#include <cvt/gfx/Font.h>
#include <cvt/gl/GLTexture.h>

namespace cvt {
	class GLTexFont : public Font {
		public:
			GLTexFont( const char* path );
			float glyphTexSize() const;
			const GLTexture& texture() const;
			int size() const;
			Recti stringBounds( const char* str, size_t n = 0 );

		private:
			GLTexFont( const GLTexFont& );
			const GLTexFont& operator=( const GLTexFont& );

			float _ptsize;
			struct GlyphMetric {
				int32_t width, height;
				int32_t bearingX, bearingY;
				int32_t advance;
			};
			GlyphMetric _metric[ 256 ];
			GLTexture _tex;
	};
}

#endif
