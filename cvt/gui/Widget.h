#ifndef CVTWIDGET_H
#define CVTWIDGET_H

#include <cvt/util/Rect.h>
#include <cvt/gui/GFX.h>
#include <cvt/gui/Events.h>


namespace cvt {
	class WidgetImpl;

	class Widget {
		public:
			Widget( const Widget* parent );
			virtual ~Widget();

			const Widget* getParent() const { return parent; };
			void getSize( int& width, int& height ) const;
			void setSize( int width, int height );
			void getPosition( int& x, int& y ) const;
			void setPosition( int x, int y );
			void setRect( const Recti& rect );
			void getRect( Recti& rect ) const;
			void setVisible( bool b );
			bool isVisible() const;
			void show() { setVisible( true ); };
			void hide() { setVisible( false ); };
			void setTitle( const std::string& title );
			void update();

			virtual void resizeEvent( ResizeEvent* event ) {};
			virtual void moveEvent( MoveEvent* event ) {};
			virtual void paintEvent( PaintEvent* event, GFX* gfx ) {};
			virtual void showEvent( ShowEvent* event ) {};
			virtual void hideEvent( HideEvent* event ) {};
			virtual void mouseMoveEvent( ) {};
			virtual void mousePressEvent() {};
			virtual void mouseReleaseEvent() {};
			virtual void mouseWheelEvent() {};
			virtual void mouseEnterEvent() {};
			virtual void mouseLeaveEvent() {};
			virtual void keyPressEvent() {};
			virtual void keyReleaseEvent() {};

		protected:
			Widget( const Widget& w );

			WidgetImpl* impl;
			const Widget* parent;
	};
}

#endif
