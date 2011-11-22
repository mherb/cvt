#include "PlotView.h"

namespace cvt {
	PlotView::PlotView() :
		_xmargin( 50 ), _ymargin( 40 ),
		_xticslabelmargin( 5 ), _yticslabelmargin( 5 ),
		_view( -5.0f, -5.0f, 10.0f, 10.0f ),
		_xtics( 1.0f ),
		_ytics( 1.0f ),
		_grid( true ),
		_xticslabel( "%.1f" ),
		_yticslabel( "%.1f" ),
		_xlabel( "x" ),
		_ylabel( "y" )
	{
	}

	PlotView::~PlotView()
	{
		_pdata.clear();
	}


	void PlotView::paintEvent( PaintEvent* , GFX* g )
	{
		int w, h;
		size( w, h );

		// fill background
		g->color() = Color::WHITE;
		g->fillRect( 0, 0, w, h );

		g->setLineWidth( 1.0f );


		String tmp;
		float sx = ( float ) ( w - 2 * _xmargin ) / _view.width;
		float sy = ( float ) ( h - 2 * _xmargin ) / _view.height;
		float xtic = sx * _xtics;
		float ytic = sy * _ytics;

		// draw grid
		if( _grid ) {
			g->setColor( Color( 0.0f, 0.0f, 0.0f, 0.1f ) );
			for( float x = _xmargin; x <= w - _xmargin + 1; x += xtic  )
				g->drawLine( x, _xmargin, x, h - _xmargin );
			for( float y = _xmargin; y <= h - _xmargin + 1; y += ytic  )
				g->drawLine( _xmargin, y, w - _xmargin, y );
		}

		// draw x/y axes
		g->color() = Color::BLACK;
		g->drawLine( _xmargin, _xmargin, _xmargin, h - _xmargin - 1 );
		g->drawLine( _xmargin, h - _xmargin, w - _xmargin - 1, h - _xmargin );


		// draw x/y tics and label
		for( float x = _xmargin, xpos = _view.x; x <= w - _xmargin + 1; x += xtic, xpos += _xtics  ) {
			g->drawLine( x, h - _xmargin, x, h - _xmargin - 5 );
			tmp.sprintf( _xticslabel.c_str(), xpos  );
			g->drawText( x - 20.0f, h - _xmargin + _xticslabelmargin, 40, 15, ALIGN_CENTER, tmp.c_str() );
		}

		for( float y = _xmargin, ypos = _view.y + _view.height; y <= h - _xmargin + 1; y += ytic, ypos -= _ytics  ) {
			g->drawLine( _xmargin, y, _xmargin + 5, y );
			tmp.sprintf( _yticslabel.c_str(), ypos  );
			g->drawText( 0, y - 10, _xmargin - _yticslabelmargin, 20, ALIGN_RIGHT | ALIGN_VCENTER, tmp.c_str() );
		}

		// Draw data
		for( std::list<PlotDataStyled>::iterator it = _pdata.begin(); it != _pdata.end(); ++it ) {
			g->setColor( ( *it )._pdata->plotColor() );
			switch(  ( *it )._pdata->plotStyle() ) {
				case PLOT_STYLE_DOTS:
					break;
				case PLOT_STYLE_LINES:
					g->setLineWidth( ( *it )._pdata->plotSize() );
					size_t n = ( *it)._data.size() / 2;
					for( size_t i = 0; i < n; i++ ) {
					//	g->drawLine( ( ( *it )._data[ i << 1 ] - _view.x ) * sx, ( ( *it )._data[ ( i << 1 ) + 1 ] - _view.x ) * sx  )
					}
					//g->drawLines( &( ( *it )._data[ 0 ] ), ( *it)._data.size() / 2 );
					break;
			}
		}


	}
}
