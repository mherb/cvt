#ifndef CVTGFXGL_H
#define CVTGFXGL_H

#include <cvt/gui/GFX.h>
#include <cvt/gl/OpenGL.h>
#include <cvt/gl/GLProgram.h>

namespace cvt {
	class GFXGL : public GFX
	{
		friend class WidgetImplWinGLX11;
		public:
			GFXGL();
			~GFXGL() {};

			virtual void fillRect( const Recti& rect );
			virtual void fillRect( int x, int y, int width, int height );
			virtual void drawText( int x, int y, const char* text );

		private:
			void updateState();
			void setViewport( int w, int h ) { width = w; height = h; };

			GLProgram progbasic;
			GLProgram progtext;
			GLuint vbuffer;
			GLuint varray;
			GLuint texfont;
			GLuint sampler;
			int width, height;
	};
}


#endif
