#ifndef CVTWIDGET_H
#define CVTWIDGET_H

#include <cvt/geom/Rect.h>
#include <cvt/gfx/GFX.h>
#include <cvt/gui/Events.h>

namespace cvt {
	class WidgetImpl;
	class GFXEngine;

	class Widget {
		friend class WidgetImplWinGLX11;
		friend class WidgetImplDefault;
		friend class WidgetContainer;
		friend class Moveable;

		public:
			Widget( bool toplevel = false );
			virtual ~Widget();

			bool isToplevel() const { return _toplevel; };
			Widget* parent() const;
			void size( int& width, int& height ) const;
			void setSize( int width, int height );
			void position( int& x, int& y ) const;
			void setPosition( int x, int y );
			void setRect( const Recti& rect );
			const Recti& rect( ) const;
			void setVisible( bool b );
			bool isVisible() const;
			void show() { setVisible( true ); };
			void hide() { setVisible( false ); };
			void raise();
			void lower();
			void setTitle( const std::string& title );
			void setMinimumSize( int width, int height );
			void setMaximumSize( int width, int height );
			void minimumSize( int& w, int& h );
			void maximumSize( int& w, int& h );
			void update();
			void update( const Recti& rect );
			void mapGlobal( int&x, int& y );
			void mapLocal( int& x, int& y );

		protected:
			virtual void resizeEvent( ResizeEvent* ) {};
			virtual void moveEvent( MoveEvent* ) {};
			virtual void paintEvent( PaintEvent* , GFX* ) {};
			virtual void showEvent( ShowEvent* ) {};
			virtual void hideEvent( HideEvent* ) {};
			virtual void mouseMoveEvent( MouseMoveEvent* ) {};
			virtual void mousePressEvent( MousePressEvent* ) {};
			virtual void mouseReleaseEvent( MouseReleaseEvent* ) {};
			virtual void mouseWheelEvent() {};
			virtual void mouseEnterEvent() {};
			virtual void mouseLeaveEvent() {};
			virtual void keyPressEvent() {};
			virtual void keyReleaseEvent() {};

			virtual void raiseChild( Widget* ) {};
			virtual void lowerChild( Widget* ) {};

			void paintChild( Widget* w, GFX* gfx, const Recti& rect ) const;

			GFXEngine* gfxEngine();

		private:
			Widget( const Widget& w );

			void setParent( Widget* parent );

			bool _toplevel;
			WidgetImpl* impl;
	};
}

#endif
