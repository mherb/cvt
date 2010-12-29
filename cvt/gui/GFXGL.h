#ifndef CVTGFXGL_H
#define CVTGFXGL_H

#include <cvt/gui/GFX.h>
#include <cvt/gl/OpenGL.h>
#include <cvt/gl/GLProgram.h>
#include <cvt/gl/GLVertexArray.h>

#include <cvt/gl/progs/GLFillRectProg.h>
#include <cvt/gl/progs/GLFillRoundRectProg.h>
#include <cvt/gl/progs/GLDrawTextProg.h>
#include <cvt/gl/progs/GLDrawImageProg.h>
#include <cvt/gl/progs/GLDrawIconProg.h>
#include <cvt/gl/progs/GLDrawModelProg.h>

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
			virtual void drawImage( int x, int y, int width, int height, const Image& img );
			virtual void drawIcon( int x, int y, Icon icon );
			virtual void drawIcons( const Vector2i* pts, size_t npts, Icon i );
			virtual int textWidth( const char* text );
			virtual void drawModel( GLModel& mdl, const Matrix4f& modelview, float near = 0.01f, float far = 100.0f );

		private:
			void updateState();

			GLFillRectProg fillrectp;
			GLFillRoundRectProg fillrrectp;
			GLDrawTextProg drawtextp;
			GLDrawImageProg drawimgp;
			GLDrawIconProg drawiconp;
			GLDrawModelProg modelp;
	};
}


#endif
