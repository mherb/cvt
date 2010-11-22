#ifndef CVTGFXGL_H
#define CVTGFXGL_H

#include <cvt/gui/GFX.h>
#include <cvt/gl/OpenGL.h>
#include <cvt/gl/GLProgram.h>
#include <cvt/gl/GLVertexArray.h>

#include <cvt/gl/progs/GLFillRectProg.h>
#include <cvt/gl/progs/GLFillRoundRectProg.h>

namespace cvt {
	class GFXGL : public GFX
	{
		friend class WidgetImplWinGLX11;

		public:
			GFXGL();
			~GFXGL();

			virtual void fillRect( const Recti& rect );
			virtual void fillRect( int x, int y, int width, int height );
			virtual void fillRoundRect( const Recti& rect, float radius );
			virtual void fillRoundRect( int x, int y, int width, int height, float radius );
			virtual void drawText( int x, int y, const char* text );
			virtual void drawImage( int x, int y, const Image& img );

		private:
			void updateState();

			GLFillRectProg fillrectp;
			GLFillRoundRectProg fillrrectp;
			GLProgram progbasic;
			GLProgram progbasictex;
			GLProgram progtext;
			GLuint texfont;
			GLVertexArray vao;
			GLBuffer vbo;
	};
}


#endif
