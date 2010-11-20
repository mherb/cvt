#ifndef CVTWIDGET_H
#define CVTWIDGET_H

#include <cvt/util/Rect.h>
#include <cvt/gui/GFX.h>
#include <cvt/gui/Events.h>

namespace cvt {
	class WidgetImpl;

	class Widget {
		friend class WidgetImplWinGLX11;
		friend class WidgetImplDefault;
		friend class WidgetContainer;

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
			void rect( Recti& rect ) const;
			void setVisible( bool b );
			bool isVisible() const;
			void show() { setVisible( true ); };
			void hide() { setVisible( false ); };
			void setTitle( const std::string& title );
			void setMinimumSize( int width, int height );
			void setMaximumSize( int width, int height );
			void minimumSize( int& w, int& h );
			void maximumSize( int& w, int& h );
			void update();
			void update( const Recti& rect );

		protected:
			virtual void resizeEvent( ResizeEvent* event ) {};
			virtual void moveEvent( MoveEvent* event ) {};
			virtual void paintEvent( PaintEvent* event, GFX* gfx ) {};
			virtual void showEvent( ShowEvent* event ) {};
			virtual void hideEvent( HideEvent* event ) {};
			virtual void mouseMoveEvent( MouseMoveEvent* event ) {};
			virtual void mousePressEvent( MousePressEvent* event ) {};
			virtual void mouseReleaseEvent( MouseReleaseEvent* event ) {};
			virtual void mouseWheelEvent() {};
			virtual void mouseEnterEvent() {};
			virtual void mouseLeaveEvent() {};
			virtual void keyPressEvent() {};
			virtual void keyReleaseEvent() {};

			void paintChild( Widget* w, GFX* gfx, const Recti& rect ) const;

		private:
			Widget( const Widget& w );

			void setParent( Widget* parent );

			bool _toplevel;
			WidgetImpl* impl;
	};
}

#endif
