#ifndef CVTWIDGETIMPL_H
#define CVTWIDGETIMPL_H

#include <cvt/geom/Rect.h>
#include <cvt/gui/Widget.h>
#include <cvt/gui/Window.h>
#include <cvt/util/String.h>

namespace cvt {
	class WidgetImpl {
		public:
			virtual				~WidgetImpl() {};
			virtual void	    setSize( int width, int height ) = 0;
			virtual void	    size( int& width, int& height ) const = 0;
			virtual void	    setPosition( int width, int height ) = 0;
			virtual void	    position( int& width, int& height ) const = 0;
			virtual void	    setRect( const Recti& rect ) = 0;
			virtual const Recti&  rect() const = 0;
			virtual void	    setVisible( bool visibility ) = 0;
			virtual bool	    isVisible() const = 0;
			virtual void	    setTitle( const String& ) {};
			virtual void		raise() = 0;
			virtual void		lower() = 0;
			virtual void	    update() = 0;
			virtual void	    update( const Recti& rect ) = 0;
			virtual void	    setMinimumSize( int w, int h ) = 0;
			virtual void	    setMaximumSize( int w, int h ) = 0;
			virtual void	    minimumSize( int& w, int& h ) = 0;
			virtual void	    maximumSize( int& w, int& h ) = 0;
			virtual void	    setParent( Widget* w ) = 0;
			virtual Widget*	    parent() const = 0;
			virtual GFXEngine* gfxEngine() = 0;
	};
}

#endif
