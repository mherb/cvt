#include <cvt/gui/Widget.h>
#include <cvt/gui/Application.h>
#include <cvt/gui/internal/WidgetImpl.h>
#include <cvt/gui/internal/WidgetImplDefault.h>

namespace cvt {
	Widget::Widget( bool toplevel ) : _toplevel( toplevel )
	{
	    if( _toplevel )
		impl = Application::instance()->registerWindow( this );
	    else
		impl = new WidgetImplDefault( this );
	}

	Widget::~Widget( )
	{
	    if( _toplevel )
		Application::instance()->unregisterWindow( impl );
	    delete impl;
	}

	Widget* Widget::parent() const
	{
	    return impl->parent();
	}

	void Widget::setParent( Widget* w )
	{
	    if( !_toplevel )
		impl->setParent( w );
	}

	void Widget::setSize( int width, int height )
	{
		impl->setSize( width, height );
	}

	void Widget::size( int& width, int& height ) const
	{
		impl->size( width, height );
	}

	void Widget::setPosition( int x, int y )
	{
		impl->setPosition( x, y );
	}

	void Widget::position( int& x, int& y ) const
	{
		impl->position( x, y );
	}

	void Widget::setRect( const Recti& rect )
	{
		impl->setRect( rect );
	}

	void Widget::rect( Recti& rect ) const
	{
		impl->rect( rect );
	}

	void Widget::setVisible( bool visibility )
	{
		impl->setVisible( visibility );
	}

	bool Widget::isVisible() const
	{
		return impl->isVisible();
	}

	void Widget::setTitle( const std::string& title )
	{
		impl->setTitle( title );
	}

	void Widget::setMinimumSize( int width, int height )
	{
	    impl->setMinimumSize( width, height );
	}

	void Widget::setMaximumSize( int width, int height )
	{
	    impl->setMaximumSize( width, height );
	}

	void Widget::minimumSize( int& w, int& h )
	{
	    impl->minimumSize( w, h );
	}

	void Widget::maximumSize( int& w, int& h )
	{
	    impl->maximumSize( w, h );
	}

	void Widget::update()
	{
	    impl->update();
	}


	void Widget::update( const Recti& rect )
	{
	    impl->update( rect );
	}

	void Widget::paintChild( Widget* w, GFX* gfx, const Recti& rect ) const
	{
		if( w->parent() != this )
			return;

		// FIXME: is rect in local coords or in child coords

		Recti viewport, newviewport;

		/* get current viewport */
	    gfx->viewport( viewport );
		/* get child rectangle */
		Recti rchild;
		w->rect( rchild );
		/* bring child rect in the local widget coordinates */
		rchild.translate( viewport.x, viewport.y );
		newviewport.intersect( rchild );

		/* set new viewport */
		gfx->setViewport( newviewport );
		/* do painting with default GFX */
		PaintEvent pe( rect.x, rect.y, rect.width, rect.height );
		gfx->setDefault();
		w->paintEvent( &pe, gfx );
		/* restore old viewport */
		gfx->setViewport( viewport );
	}

}
