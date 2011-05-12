#ifndef CVT_IFILTERWINDOW_H
#define CVT_IFILTERWINDOW_H

#include <cvt/gui/Window.h>
#include "IFilterViewPlug.h"

namespace cvt {
	class IFilterWindow : public Window
	{
		public:
		   IFilterWindow( const String& name );
		   ~IFilterWindow();

			virtual void mousePressEvent( MousePressEvent* );
			virtual void mouseMoveEvent( MouseMoveEvent* );
			virtual void mouseReleaseEvent( MouseReleaseEvent* );
			virtual void paintEvent( PaintEvent* , GFX* );
		private:
			IFilterViewPlug* _cplug;
			Vector2i		 _cdst;
	};

	inline IFilterWindow::IFilterWindow( const String& name ) : Window( name ), _cplug( NULL )
	{
	}

	inline IFilterWindow::~IFilterWindow()
	{
	}

	inline void IFilterWindow::mousePressEvent( MousePressEvent* e )
	{
		Widget* child1 = this;
		Widget*	child2 = NULL;
		IFilterViewPlug* plug;

		while( ( child2 = child1->childAt( e->x, e->y ) ) )
			child1 = child2;

		if( ( plug = dynamic_cast<IFilterViewPlug*>( child1 ) ) ) {
			_cplug = plug;
			_cdst.set( e->x, e->y );
			_cplug->setConnected( true );
		} else {
			Window::mousePressEvent( e );
		}
	}

	inline void IFilterWindow::mouseMoveEvent( MouseMoveEvent* e )
	{
		if( _cplug ) {
			_cdst.set( e->x, e->y );
			update();
		} else
			Window::mouseMoveEvent( e );
	}

	inline void IFilterWindow::mouseReleaseEvent( MouseReleaseEvent* e )
	{
		if( _cplug ) {
			_cplug->setConnected( false );
			_cplug = NULL;
			update();
		} else
			Window::mouseReleaseEvent( e );
	}


	inline void IFilterWindow::paintEvent( PaintEvent* , GFX* g )
	{
		Recti r = rect();
		g->color().set( 0.4f, 0.4f, 0.4f, 1.0f );
		g->fillRect( 0, 0, r.width, r.height );
		if( _cplug ) {
			Vector2i csrc = _cplug->center();
			Pathf p;
			p.moveTo( csrc.x, csrc.y );
			float len = Math::abs( _cdst.x - csrc.x ) * ( _cplug->isInput() ? -0.75f : 0.75f );
			p.curveTo( csrc.x + len, csrc.y, _cdst.x - len, _cdst.y, _cdst.x, _cdst.y );
			g->setLineWidth( 0.0f );
			g->color().set( 0.8f, 0.9f, 0.1f, 1.0f );
			g->strokePath( p );
		}
		int w, h;
		size( w, h );
		paintChildren( g, r );
	}
}

#endif
