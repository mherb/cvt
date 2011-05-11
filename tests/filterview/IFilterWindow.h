#ifndef CVT_IFILTERWINDOW_H
#define CVT_IFILTERWINDOW_H

#include <cvt/gui/Window.h>

namespace cvt {
	class IFilterWindow : public Window
	{
		public:
		   IFilterWindow( const String& name );
		   ~IFilterWindow();

			virtual void mouseMoveEvent( MouseMoveEvent* );
			virtual void mousePressEvent( MousePressEvent* );
			virtual void mouseReleaseEvent( MouseReleaseEvent* );
			virtual void paintEvent( PaintEvent* , GFX* );
		private:
			Vector2i _cpt1, _cpt2;
			bool _cpts;
	};

	inline IFilterWindow::IFilterWindow( const String& name ) : Window( name ), _cpts( false )
	{
	}

	inline IFilterWindow::~IFilterWindow()
	{
	}

	inline void IFilterWindow::mousePressEvent( MousePressEvent* e )
	{
		if( e->button() == 1 ) {
			_cpts = true;
			_cpt1.set( e->x, e->y );
		}
	}

	inline void IFilterWindow::mouseMoveEvent( MouseMoveEvent* e )
	{
		if( _cpts ) {
			_cpt2.set( e->x, e->y );
			update();
		}
	}

	inline void IFilterWindow::mouseReleaseEvent( MouseReleaseEvent* e )
	{
		if( _cpts ) {
			_cpts = false;
			update();
		}
	}


	inline void IFilterWindow::paintEvent( PaintEvent* pe , GFX* g )
	{
		Window::paintEvent( pe, g  );
		if( _cpts ) {
			Pathf p;
			p.moveTo( _cpt1.x, _cpt1.y );
			float len = 0.5f * ( _cpt2.x - _cpt1.x );
			p.curveTo( _cpt1.x + len, _cpt1.y, _cpt2.x - len, _cpt2.y, _cpt2.x, _cpt2.y );
			g->setLineWidth( 0.0f );
			g->color().set( 0.0f, 1.0f, 0.0f, 1.0f );
			g->strokePath( p );
		}
	}
}

#endif
