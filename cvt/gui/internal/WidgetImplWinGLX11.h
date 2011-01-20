#ifndef CVTWIDGETIMPLWINGLX11_H
#define CVTWIDGETIMPLWINGLX11_H

#include <cvt/gui/internal/WidgetImpl.h>
#include <cvt/gui/internal/X.h>
#include <cvt/gui/Window.h>
#include <cvt/gui/GFXGL.h>
#include <cvt/math/Math.h>
#include <cvt/math/Vector.h>
#include <deque>

namespace cvt {

	class WidgetImplWinGLX11 : public WidgetImpl
	{
		friend class ApplicationX11;
		friend class X11Handler;

		public:
			WidgetImplWinGLX11( ::Display* dpy, ::GLXContext context, ::XVisualInfo* visinfo, Widget* _window, std::deque<WidgetImplWinGLX11*>* updates );
			~WidgetImplWinGLX11();
			virtual void setTitle( const std::string& title );
			virtual void setRect( const Recti& rect );
			virtual void rect( Recti& rect ) const { rect = _rect; };
			virtual void size( int& width, int& height ) const;
			virtual void setSize( int width, int height );
			virtual void position( int& x, int& y ) const;
			virtual void setPosition( int x, int y );
			virtual void setVisible( bool b );
			virtual bool isVisible( ) const { return visible; };
			virtual void raise();
			virtual void lower();
			virtual void update();
			virtual void update( const Recti& rect );
			virtual void setMinimumSize( int w, int h );
			virtual void setMaximumSize( int w, int h );
			virtual void minimumSize( int& w, int& h );
			virtual void maximumSize( int& w, int& h );
			virtual void setParent( Widget* ) {};
			virtual Widget* parent( ) const { return NULL; };

		private:
			void paintEvent( PaintEvent* event );
			void resizeEvent( ResizeEvent* event );
			void moveEvent( MoveEvent* event );
			void showEvent( ShowEvent* event );
			void hideEvent( HideEvent* event );

		private:
			Widget* _widget;
			::Display* dpy;
			::Window win;
			::GLXContext ctx;
			bool visible;
			Recti _rect;
			GFXGL* gfx;
			bool needsupdate;
			Vector2<int> _minSize;
			Vector2<int> _maxSize;
			std::deque<WidgetImplWinGLX11*>* _updateq;
		};
}

#endif
