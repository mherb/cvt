#include <cvt/gui/Application.h>
#include <cvt/gui/Window.h>
#include <cvt/math/Spline2.h>

using namespace cvt;

class SplineUI : public Window
{
	public:
	SplineUI() : Window("SplineUI"), _selection( -1 ), _spline( 10, 10, 10, 40, 100, 40, 100, 10 )
	{
	}

#if 1
	void drawSpline( GFX* g, const Spline2f& spl )
	{
//#define TEPS 0.01f
#define TOLERANCE 0.25f

		size_t n;
		float t[ 2 ], alpha;
		n = spl.inflectionPoints( t );
		Spline2f a, b, c;

			std::vector<Vector2f> lines;
			if( n )
				spl.split( a, b, t[ 0 ] );
			else
				a = spl;

			lines.push_back( a[ 0 ] );
			alpha =  a.flattenFirst( TOLERANCE );
			alpha =  Math::min( a.flatten( TOLERANCE ), alpha );
			a.remove( alpha );
			lines.push_back( a[ 0 ] );

			while( 1 ) {
				lines.push_back( a[ 0 ] );
				alpha =  a.flatten( TOLERANCE );
				if( alpha < 1 ) {
					a.remove( alpha );
					lines.push_back( a[ 0 ] );
				} else {
					lines.push_back( a[ 3 ] );
					break;
				}
			}

			if( n ) {
				if( n == 2 ) {
					a = b;
					a.split( b, c,  ( t[ 1 ] - t[ 0 ] ) / ( 1.0f - t[ 0 ] ) );
				}
				lines.push_back( b[ 0 ] );
				alpha =  b.flattenFirst( TOLERANCE );
				b.remove( alpha );
				lines.push_back( b[ 0 ] );
				while( 1 ) {
					lines.push_back( b[ 0 ] );
					alpha =  b.flatten( TOLERANCE );
					if( alpha < 1 ) {
						b.remove( alpha );
						lines.push_back( b[ 0 ] );
					} else {
						lines.push_back( b[ 3 ] );
						break;
					}
				}
				if( n == 2 ) {
					lines.push_back( c[ 0 ] );
					alpha =  c.flattenFirst( TOLERANCE );
					c.remove( alpha );
					lines.push_back( c[ 0 ] );
					while( 1 ) {
						lines.push_back( c[ 0 ] );
						alpha =  c.flatten( TOLERANCE );
						if( alpha < 1 ) {
							c.remove( alpha );
							lines.push_back( c[ 0 ] );
						} else {
							lines.push_back( c[ 3 ] );
							break;
						}
					}
				}
			}

			g->color().set( 0.0f, 1.0f, 0.0f, 0.5f );
			g->drawLines( &lines[ 0 ], lines.size() );


/*			g->fillRect( a[ 3 ].x - 4, a[ 3 ].y - 4,  8, 8 );
			if( n == 2 ) {
				Vector2f pt;
				b.samplePoint( pt, ( t[ 1 ] - t[ 0 ] ) / ( 1.0f - t[ 0 ] ) );
				g->fillRect( pt.x - 4, pt.y - 4,  8, 8 );
			}*/


	}
#endif

	void paintEvent( PaintEvent* e, GFX* g )
	{
		Window::paintEvent( e, g );

	/*	g->color().set( 1.0f, 1.0f, 1.0f, 1.0f );
		for( float t = 0.0f; t < 1.0f; t += 0.01f ) {
			Vector2f pt;
			_spline.samplePoint( pt, t );
			g->fillRoundRect( pt.x - 1, pt.y - 1,  2, 2, 1.0f );
		}*/

		g->color().set( 1.0f, 0.0f, 0.0f, 0.5f );
		g->drawLine( _spline[ 0 ].x, _spline[ 0 ].y, _spline[ 1 ].x, _spline[ 1 ].y );
		g->drawLine( _spline[ 3 ].x, _spline[ 3 ].y, _spline[ 2 ].x, _spline[ 2 ].y );
		g->fillRoundRect( _spline[ 0 ].x - 5, _spline[ 0 ].y - 5,  10, 10, 5.0f );
		g->fillRoundRect( _spline[ 1 ].x - 5, _spline[ 1 ].y - 5,  10, 10, 5.0f );
		g->fillRoundRect( _spline[ 2 ].x - 5, _spline[ 2 ].y - 5,  10, 10, 5.0f );
		g->fillRoundRect( _spline[ 3 ].x - 5, _spline[ 3 ].y - 5,  10, 10, 5.0f );

		{
			size_t n;
			float t[ 2 ];
			n = _spline.inflectionPoints( t );
			g->color().set( 0.0f, 1.0f, 0.0f, 0.5f );
			for( size_t i= 0; i < n; i++ ) {
				Vector2f pt;
				if( t[ i ] < 0.0f || t[ i ] > 1.0f )
					continue;
				_spline.samplePoint( pt, t[ i ] );
				g->fillRoundRect( pt.x - 2, pt.y - 2,  4, 4, 2.0f );
			}

			g->color().set( 0.0f, 0.0f, 1.0f, 0.25f );

			Spline2f a, b;
			if( n ) {
				_spline.split( a, b, t[ 0 ] );
				g->fillRect( a[ 3 ].x - 4, a[ 3 ].y - 4,  8, 8 );
			}
			if( n == 2 ) {
				Vector2f pt;
				b.samplePoint( pt, ( t[ 1 ] - t[ 0 ] ) / ( 1.0f - t[ 0 ] ) );
				g->fillRect( pt.x - 4, pt.y - 4,  8, 8 );
			}

		}
		drawSpline( g, _spline );

	}

	void mousePressEvent( MousePressEvent* event )
	{
		Vector2f d, p;
		p.set( event->x, event->y );
		d = _spline[ 0 ] - p;
		if( d.length() < 10.0f ) {
			_selection = 0;
			return;
		}
		d = _spline[ 1 ] - p;
		if( d.length() < 10.0f ) {
			_selection = 1;
			return;
		}
		d = _spline[ 2 ] - p;
		if( d.length() < 10.0f ) {
			_selection = 2;
			return;
		}
		d = _spline[ 3 ] - p;
		if( d.length() < 10.0f ) {
			_selection = 3;
			return;
		}
		_selection = -1;
	}

	void mouseMoveEvent( MouseMoveEvent* event )
	{
		if( _selection < 0 )
			return;
		Vector2f p;
		p.set( event->x, event->y );
		_spline[ _selection ] = p;
		update();
	}

	void mouseReleaseEvent( MouseReleaseEvent* e )
	{
		_selection = -1;
	}

	private:
		int _selection;
		Spline2f _spline;
};

int main()
{
	SplineUI ui;
	ui.setSize( 640, 480 );
	ui.show();
	Application::run();
	return 0;
}
