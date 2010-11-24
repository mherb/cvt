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

	void WidgetContainer::resizeEvent( ResizeEvent* )
	{
		resizeChildren();
	}

	void WidgetContainer::resizeChildren()
	{
		Recti r;
		Recti rself;

		rect( rself );
		if( isToplevel() )
			rself.setPosition( 0, 0 );

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
			it->second.rect( r, rself );
			w->setRect( r );
			++it;
		}
	}

	void WidgetContainer::moveEvent( MoveEvent* me )
	{
		int dx = me->x - me->oldx;
		int dy = me->y - me->oldy;
		moveChildren( dx, dy );
	}

	void WidgetContainer::moveChildren( int dx, int dy )
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
			r.x += dx;
			r.y += dy;
			w->setRect( r );
			++it;
		}
	}

	void WidgetContainer::paintEvent( PaintEvent* , GFX* gfx )
	{
		Recti r;
		rect( r );
		paintChildren( gfx, r );
	}

	void WidgetContainer::paintChildren( GFX* gfx, const Recti& r )
	{

		ChildList::reverse_iterator it = _children.rbegin();
		while( it != _children.rend() )
		{
			Widget* w = it->first;
			if( w->parent() != this ) {
				ChildList::iterator it2 = it.base();
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
		int x, y;

		event->position( x, y );
		mapGlobal( x, y );
		_activeWidget = childAt( x, y );

		if( _activeWidget ) {
			mapGlobal( event->x, event->y );
			_activeWidget->mapLocal( event->x, event->y );
			_activeWidget->mousePressEvent( event );
		}

	}

	void WidgetContainer::mouseMoveEvent( MouseMoveEvent* event )
	{
		if( _activeWidget ) {
			mapGlobal( event->x, event->y );
			_activeWidget->mapLocal( event->x, event->y );
			_activeWidget->mouseMoveEvent( event );
		}
	}

	void WidgetContainer::mouseReleaseEvent( MouseReleaseEvent* event )
	{
		if( _activeWidget ) {
			mapGlobal( event->x, event->y );
			_activeWidget->mapLocal( event->x, event->y );
			_activeWidget->mouseReleaseEvent( event );
		}
		_activeWidget = NULL;
	}

};
