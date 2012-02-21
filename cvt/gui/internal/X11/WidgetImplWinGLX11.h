/*
			CVT - Computer Vision Tools Library

 	 Copyright (c) 2012, Philipp Heise, Sebastian Klose

 	THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 	KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 	IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 	PARTICULAR PURPOSE.
 */
#ifndef CVT_WIDGETIMPLWINGLX11_H
#define CVT_WIDGETIMPLWINGLX11_H

#include <cvt/gui/internal/WidgetImpl.h>
#include <cvt/gui/internal/X11/X.h>
#include <cvt/gui/internal/X11/GLXContext.h>
#include <cvt/gui/Window.h>
#include <cvt/gfx/GFX.h>
#include <cvt/math/Math.h>
#include <cvt/math/Vector.h>
#include <cvt/gl/GFXEngineGL.h>
#include <cvt/util/String.h>
#include <deque>

namespace cvt {

	class WidgetImplWinGLX11 : public WidgetImpl
	{
		friend class ApplicationX11;
		friend class X11Handler;

		public:
			WidgetImplWinGLX11( ::Display* dpy, const GLXContext* sharecontext, Widget* _window, std::deque<WidgetImplWinGLX11*>* updates );
			~WidgetImplWinGLX11();
			virtual void setTitle( const String& title );
			virtual void setRect( const Recti& rect );
			virtual const Recti& rect( ) const { return _rect; };
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
			virtual GFXEngine* gfxEngine();

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
			GLXContext* _ctx;
			bool visible;
			Recti _rect;
			GFXEngineGL* gfxgl;
			bool needsupdate;
			Vector2<int> _minSize;
			Vector2<int> _maxSize;
			std::deque<WidgetImplWinGLX11*>* _updateq;
		};
}

#endif
