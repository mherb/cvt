#include <cvt/gui/Application.h>
#include <cvt/gui/Window.h>
#include <cvt/gui/ToggleButton.h>
#include <cvt/util/String.h>
#include <cvt/io/xml/XMLDocument.h>
#include <cvt/gfx/IConvert.h>
#include <cvt/geom/Rect.h>
#include <cvt/math/Vector.h>
#include <cvt/math/Math.h>
#include <cvt/gl/progs/GLTexMLSProg.h>

#include <iostream>
#include <vector>

using namespace cvt;

class GLMLS : public Window
{
	public:
	GLMLS( const String& imgstr ) : Window("GLMLS"), _selection( -1 ), _toggle( String("Show displacements"), true )
	{
		Image img;

		img.load( imgstr.c_str() );
		_glimage.reallocate( img.width(), img.height(), IFormat::RGBA_UINT8, IALLOCATOR_GL );
		try {
			IConvert::convert( _glimage, img );
		} catch( Exception e ) {
			std::cerr << "Conversion error: " << e.what() << std::endl;
		}

		WidgetLayout wl;
		wl.setAnchoredRight( 10, 200 );
		wl.setAnchoredTop( 10, 20 );
		addWidget( &_toggle, wl );

		_prog.bind();
		_prog.setAlpha( 1.0 );
		_prog.unbind();

		_displacements.push_back( Vector4f( 0.1, 0.1, 0.1, 0.1 ));
		_displacements.push_back( Vector4f( 0.1, 0.9, 0.1, 0.9 ));
		_displacements.push_back( Vector4f( 0.9, 0.1, 0.9, 0.1 ));
		_displacements.push_back( Vector4f( 0.9, 0.9, 0.9, 0.9 ));
	//	_displacements.push_back( Vector4f( 0.5, 0.5, 0.6, 0.6 ));
		updateDisplacements();
	}

	void updateDisplacements()
	{
		_prog.setDisplacements( &_displacements[ 0 ], _displacements.size() );
		update();
	}

	~GLMLS()
	{
	}

	void paintEvent( PaintEvent* e, GFX* g )
	{
		Window::paintEvent( e, g );

		int w, h;
		float aspect;
		size( w, h );

		/* Draw image */

		aspect = ( ( float ) _glimage.width() / ( float ) _glimage.height() );

		if(  h * aspect > ( float ) w  ) {
			_dw = w;
			_dh = _dw / aspect;
		} else {
			_dh = h;
			_dw = aspect * _dh;
		}

		Matrix4f proj;
		GL::ortho( proj, 0, ( float ) w, 0, ( float ) h  );
		_prog.bind();
		_prog.setProjection( proj );
		_prog.drawImage( 0, 0, _dw, _dh, _glimage );
		_prog.unbind();

		/* Draw displacements */
		if( _toggle.state() ) {
			g->color() = Color::BLUE;
			g->color().setAlpha( 0.5f );
			for( size_t i = 0; i < _displacements.size(); i++) {
				Vector4f dp = _displacements[ i ];
				g->drawIcon( dp.x * _dw - 8, dp.y * _dh - 8, GFX::ICON_CIRCLE );
			}
			g->color() = Color::YELLOW;
			g->color().setAlpha( 0.5f );
			for( size_t i = 0; i < _displacements.size(); i++) {
				Vector4f dp = _displacements[ i ];
				g->drawIcon( dp.z * _dw - 8, dp.w * _dh - 8, GFX::ICON_CIRCLE );
			}
		}

	}

	void mousePressEvent( MousePressEvent* event )
	{
		Vector2f pt( event->x, event->y );
		Recti r( 0, 0, _dw, _dh );
		if( r.contains( event->x, event->y ) ) {
			for( size_t i = 0; i < _displacements.size(); i++) {
				Vector4f dp = _displacements[ i ];
				Vector2f pt1( dp.x * _dw, dp.y * _dh );
				Vector2f pt2( dp.z * _dw, dp.w * _dh );

				if( ( pt1 - pt ).length() < 5.0f ) {
					_selection = i;
					_orig = true;
					return;
				}

				if( ( pt2 - pt ).length() < 5.0f ) {
					_selection = i;
					_orig = false;
					return;
				}
			}
			float nx = ( float ) event->x / ( float ) _dw;
			float ny = ( float ) event->y / ( float ) _dh;
			_displacements.push_back( Vector4f( nx, ny, nx, ny ) );
			_selection = -1;
			updateDisplacements();
		} else {
			WidgetContainer::mousePressEvent( event );
		}
	}

	void mouseMoveEvent( MouseMoveEvent* event )
	{
		if( _selection >= 0 ) {
			if( _orig ) {
				_displacements[ _selection ].x = ( float ) event->x / ( float ) _dw;
				_displacements[ _selection ].y = ( float ) event->y / ( float ) _dh;
			} else {
				_displacements[ _selection ].z = ( float ) event->x / ( float ) _dw;
				_displacements[ _selection ].w = ( float ) event->y / ( float ) _dh;
			}
			updateDisplacements();
		}
	}

	void mouseReleaseEvent( MouseReleaseEvent* event )
	{
		_selection = -1;
		WidgetContainer::mouseReleaseEvent( event );
	}

	private:
		Image _glimage;
		int _selection;
		bool _orig;
		GLTexMLSProg _prog;
		int _dw, _dh;
		ToggleButton _toggle;
		std::vector<Vector4f> _displacements;
};

int main( int argc, char** argv )
{
	if( argc != 2 ) {
		std::cout << "usage: " << argv[ 0 ] << " image" << std::endl;
		return 1;
	}
	GLMLS ui( argv[ 1 ] );
	ui.setSize( 640, 480 );
	ui.show();
	Application::run();
	return 0;
}
