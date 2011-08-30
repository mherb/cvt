#include <cvt/gui/Application.h>
#include <cvt/gui/Window.h>
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
	GLMLS( const String& imgstr ) : Window("GLMLS")
	{
		Image img;

		img.load( imgstr.c_str() );
		_glimage.reallocate( img.width(), img.height(), IFormat::RGBA_UINT8, IALLOCATOR_GL );
		try {
			IConvert::convert( _glimage, img );
		} catch( Exception e ) {
			std::cerr << "Conversion error: " << e.what() << std::endl;
		}

		_prog.bind();
		_prog.setAlpha( 1.0 );
		_prog.unbind();

		Vector4f vec[ 5 ];
		vec[ 0 ].set( 0, 0, 0, 0 );
		vec[ 1 ].set( 0, 1, 0, 1 );
		vec[ 2 ].set( 1, 0, 1, 0 );
		vec[ 3 ].set( 1, 1, 1, 1 );
		vec[ 4 ].set( 0.5, 0.5, 0.6, 0.6 );
		_prog.setDisplacements( &vec[ 0 ], 5 );

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
	}

	void mousePressEvent( MousePressEvent* event )
	{
	}

	void mouseMoveEvent( MouseMoveEvent* event )
	{
	}

	void mouseReleaseEvent( MouseReleaseEvent* e )
	{
	}

	private:
		Image _glimage;
		int _selection;
		GLTexMLSProg _prog;
		int _dw, _dh;
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
