#ifndef CVTWIDGETIMPL_H
#define CVTWIDGETIMPL_H

#include <cvt/util/Rect.h>
#include <cvt/gui/Widget.h>
#include <cvt/gui/Window.h>

namespace cvt {
	class WidgetImpl {
		public:
			WidgetImpl( Widget* w ) : widget( w ) {};

			virtual void setSize( int width, int height ) = 0;
			virtual void getSize( int& width, int& height ) const = 0;
			virtual void setPosition( int width, int height ) = 0;
			virtual void getPosition( int& width, int& height ) const = 0;
			virtual void setRect( const Recti& rect ) = 0;
			virtual void getRect( Recti& rect ) const = 0;
			virtual void setVisible( bool visibility ) = 0;
			virtual bool isVisible() const = 0;
			virtual void setTitle( const std::string& title ) {};
			virtual void update() = 0;

			Widget* widget;
	};
}

#endif
