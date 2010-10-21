#ifndef CVTWIDGETIMPLWINGLX11_H
#define CVTWIDGETIMPLWINGLX11_H

#include <cvt/gui/internal/WidgetImpl.h>
#include <cvt/gui/internal/X.h>
#include <cvt/gui/Window.h>
#include <cvt/gui/GFXGL.h>
#include <queue>

namespace cvt {

	class WidgetImplWinGLX11 : public WidgetImpl
	{
		friend class ApplicationX11;

		public:
			WidgetImplWinGLX11( ::Display* dpy, ::GLXContext context, ::XVisualInfo* visinfo, Window* _window, std::queue<WidgetImplWinGLX11*>* uq );
			~WidgetImplWinGLX11();
			virtual void setTitle( const std::string& title );
			virtual void setRect( const Recti& rect );
			virtual void getRect( Recti& _rect ) const { _rect = rect; };
			virtual void getSize( int& width, int& height ) const;
			virtual void setSize( int width, int height );
			virtual void getPosition( int& x, int& y ) const;
			virtual void setPosition( int x, int y );
			virtual void setVisible( bool b );
			virtual bool isVisible( ) const { return visible; };
			virtual void update();

		private:
			void paintEvent( PaintEvent* event );
			void resizeEvent( ResizeEvent* event );
			void moveEvent( MoveEvent* event );
			void showEvent( ShowEvent* event );
			void hideEvent( HideEvent* event );

		private:
			::Display* dpy;
			::Window win;
			::GLXContext ctx;
			bool visible;
			Recti rect;
			GFXGL* gfx;
			bool needsupdate;
			std::queue<WidgetImplWinGLX11*>* updateq;
		};
}

#endif
