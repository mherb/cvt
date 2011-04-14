#include <cvt/gui/Application.h>
#include <cvt/gui/Window.h>
#include <cvt/math/Spline2.h>
#include <cvt/util/String.h>
#include <cvt/io/ImageIO.h>
#include <cvt/gfx/IConvert.h>
#include <cvt/geom/Rect.h>
#include <cvt/math/Vector.h>

#include <iostream>
#include <vector>

using namespace cvt;

class FaceUI : public Window
{
	public:
	FaceUI( const String& str ) : Window("Face Annotator"), _selection( - 1 )
	{
		Image img;
		ImageIO::loadPNG( img, str.c_str() );
		_glimage.reallocate( img.width(), img.height(), IFormat::RGBA_UINT8, IALLOCATOR_GL );
		try {
			IConvert::convert( _glimage, img );
		} catch( Exception e ) {
			std::cerr << "Conversion error: " << e.what() << std::endl;
		}
		update();
	}

	void paintEvent( PaintEvent* e, GFX* g )
	{
		Window::paintEvent( e, g );

		int w, h;
		float aspect;
		size( w, h );

		aspect = ( ( float ) _glimage.width() / ( float ) _glimage.height() );

		if(  h * aspect > ( float ) w  ) {
			_dw = w;
			_dh = _dw / aspect;
		} else {
			_dh = h;
			_dw = aspect * _dh;
		}
		g->drawImage( 0, 0, _dw, _dh, _glimage );

		g->color().set( 0.0f, 1.0f, 0.0f, 0.5f );
		for( int i =0, end = _pts.size(); i < end; i++ ) {
			float dx = ( _pts[ i ].x + 0.5f ) * _dw;
			float dy = ( _pts[ i ].y + 0.5f ) * _dh;
			g->fillRoundRect( dx - 4, dy - 4, 8, 8, 4 );
		}
	}

	void mousePressEvent( MousePressEvent* event )
	{
		Rectf r( 0, 0, _dw, _dh );

		float x = ( ( event->x / _dw ) - 0.5f );
		float y = ( ( event->y / _dh ) - 0.5f );


		for( int i =0, end = _pts.size(); i < end; i++ ) {
			float dx = ( _pts[ i ].x + 0.5f ) * _dw;
			float dy = ( _pts[ i ].y + 0.5f ) * _dh;
			Rectf r( dx - 4, dy - 4, 8, 8 );
			if( r.contains( event->x, event->y ) ) {
				_selection = i;
				_lx = event->x;
				_ly = event->y;
				return;
			}
		}
		_selection = -1;
		if( r.contains( event->x, event->y ) ) {
			std::cout << "x=\"" << x << "\" "
					  << "y=\"" << y << "\" " << std::endl;
			_pts.push_back( Point2f( x, y ) );
			update();
		}
	}

	void mouseMoveEvent( MouseMoveEvent* event )
	{
		if( _selection >= 0 ) {
			float dx = ( event->x - _lx ) / _dw;
			float dy = ( event->y - _ly ) / _dh;
			_pts[ _selection ] += Point2f( dx, dy );
			_lx = event->x;
			_ly = event->y;
			update();
		}
	}

	void mouseReleaseEvent( MouseReleaseEvent* e )
	{
		if( _selection >= 0 ) {
			std::cout << "x=\"" << _pts[ _selection ].x << "\" "
				<< "y=\"" << _pts[ _selection ].y << "\" " << std::endl;
			_selection = -1;
		}
	}

	private:
		Image _glimage;
		float _dw, _dh;
		std::vector<Point2f> _pts;
		int _selection;
		float _lx, _ly;
};

int main( int argc, char** argv )
{
	FaceUI ui( argv[ 1 ] );
	ui.setSize( 640, 480 );
	ui.show();
	Application::run();
	return 0;
}
