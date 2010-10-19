#include <cvt/gui/Widget.h>
#include <cvt/gui/Application.h>
#include <cvt/gui/internal/WidgetImpl.h>

namespace cvt {
	Widget::Widget( const Widget* _parent ) : parent( _parent )
	{
		impl = Application::instance()->getWidgetImpl( this );
	}

	Widget::~Widget( )
	{
		delete impl;
	}

	void Widget::setSize( int width, int height )
	{
		impl->setSize( width, height );
	}

	void Widget::getSize( int& width, int& height ) const
	{
		impl->getSize( width, height );
	}

	void Widget::setPosition( int x, int y )
	{
		impl->setPosition( x, x );
	}

	void Widget::getPosition( int& x, int& y ) const
	{
		impl->getPosition( x, y );
	}

	void Widget::setRect( const Recti& rect )
	{
		impl->setRect( rect );
	}

	void Widget::getRect( Recti& rect ) const
	{
		impl->getRect( rect );
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

	void Widget::update()
	{
		impl->update();
	}


}
