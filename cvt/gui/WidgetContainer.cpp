#include <cvt/gui/WidgetContainer.h>

namespace cvt {
	WidgetContainer::WidgetContainer()
	{

	}

	WidgetContainer::WidgetContainer( bool toplevel ) : Widget( toplevel ), _activeWidget( NULL )
	{

	}

	WidgetContainer::~WidgetContainer()
	{
		_children.clear();
	}

	Widget* WidgetContainer::childAt( int x, int y )
	{
		ChildList::iterator it = _children.begin();
		while( it != _children.end() )
		{
			Widget* w = it->first;
			if( w->parent() != this ) {
				ChildList::iterator it2 = it;
				++it;
				_children.erase( it2 );
				continue;
			}

			Recti r;
			w->rect( r );
			if( r.contains( x, y ) )
				return w;
			++it;
		}
		return NULL;
	}

	void WidgetContainer::removeWidget( Widget* w  )
	{
		ChildList::iterator it = _children.begin();
		while( it != _children.end() )
		{
			Widget* widget = it->first;
			if( widget == w ) {
				_children.erase( it );
				return;
			}
			++it;
		}
	}

	void WidgetContainer::resizeEvent( ResizeEvent* event )
	{
		resizeChildren();
	}

	void WidgetContainer::resizeChildren()
	{
		Recti r;
		int width, height;

		size( width, height );

		ChildList::iterator it = _children.begin();
		while( it != _children.end() )
		{
			Widget* w = it->first;
			if( w->parent() != this ) {
				ChildList::iterator it2 = it;
				++it;
				_children.erase( it2 );
				continue;
			}

			w->rect( r );
			it->second.rect( r, width, height );
			w->setRect( r );
			++it;
		}
	}

	void WidgetContainer::paintEvent( PaintEvent* e, GFX* gfx )
	{
		Recti r;
		rect( r );
		paintChildren( gfx, r );
	}

	void WidgetContainer::paintChildren( GFX* gfx, const Recti& r )
	{

		ChildList::iterator it = _children.begin();
		while( it != _children.end() )
		{
			Widget* w = it->first;
			if( w->parent() != this ) {
				ChildList::iterator it2 = it;
				++it;
				_children.erase( it2 );
				continue;
			}
			if( w->isVisible() ) {
				Recti rc;
				w->rect( rc );
				rc.intersect( r );
				paintChild( w, gfx, rc );
			}
			++it;
		}

	}

	void WidgetContainer::mousePressEvent( MousePressEvent* event )
	{
		_activeWidget = childAt( event->x, event->y );
		if( _activeWidget ) {
			int cx, cy;
			_activeWidget->position( cx, cy );
			event->x -= cx;
			event->y -= cy;
			_activeWidget->mousePressEvent( event );
		}

	}

	void WidgetContainer::mouseMoveEvent( MouseMoveEvent* event )
	{
		if( _activeWidget ) {
			int cx, cy;
			_activeWidget->position( cx, cy );
			event->x -= cx;
			event->y -= cy;
			_activeWidget->mouseMoveEvent( event );
		}
	}

	void WidgetContainer::mouseReleaseEvent( MouseReleaseEvent* event )
	{
		if( _activeWidget ) {
			int cx, cy;
			_activeWidget->position( cx, cy );
			event->x -= cx;
			event->y -= cy;
			_activeWidget->mouseReleaseEvent( event );
		}
		_activeWidget = NULL;
	}

};
