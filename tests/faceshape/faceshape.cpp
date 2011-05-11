#include <cvt/gui/Application.h>
#include <cvt/gui/Window.h>
#include <cvt/gfx/Image.h>
#include <cvt/io/ImageIO.h>
#include <cvt/math/Vector.h>
#include <vector>
#include <iostream>
#include <fstream>
#include <string>

using namespace cvt;

struct ASFEntry
{
	float relx, rely;
	int num, from, to;
};

#define SCALE 2

class FaceShapeWin : public Window
{
	public:
	FaceShapeWin( const std::string& pngfile, const std::string& asffile ) : Window( asffile.c_str() )
	{
		/* read png stuff */
		Image png;
		ImageIO::loadPNG( png, pngfile );
		_imgface.reallocate( png.width(), png.height(), IFormat::RGBA_UINT8, IALLOCATOR_GL );
		png.convert( _imgface );

		/* read asf stuff */
		std::vector<ASFEntry> asfentries;
		std::ifstream file;
		file.open( asffile.c_str() );
		while( !file.eof() ) {
			ASFEntry x;
			std::string rest;
			int dummy;
			file >> dummy;
			file >> dummy;
			file >> x.relx >> x.rely >> x.num >> x.from >> x.to;
			std::getline( file, rest );
			asfentries.push_back( x );
//			std::cout << x.relx << " " << x.rely << " " << x.num << std::endl;
		}
		file.close();
		for( int i =0; i < asfentries.size(); i++ ) {
			Vector2f x( asfentries[ i ].relx * png.width() * SCALE, asfentries[ i ].rely * png.height() * SCALE );
			Vector2i k( asfentries[ i ].relx * ( float ) png.width() * SCALE - 8, asfentries[ i ].rely * ( float ) png.height() * SCALE - 8 );
			_pts.push_back( x );
			_ptsi.push_back( k );
			int to = asfentries[ i ].to;
			x.set( asfentries[ to ].relx * png.width() * SCALE, asfentries[ to ].rely * png.height() * SCALE );
//			k.set( asfentries[ to ].relx * ( float ) png.width(), asfentries[ to ].rely * ( float ) png.height() );
			_pts.push_back( x );
//			_ptsi.push_back( k );
		}
	}

	void paintEvent( PaintEvent* event, GFX* gfx )
	{
		int w, h;
		size( w, h );
		gfx->color().set( 0.6f, 0.6f, 0.6f, 1.0f );
		gfx->fillRect( 0, 0, w, h );

		gfx->drawImage( 0, 0, _imgface.width() * SCALE, _imgface.height() * SCALE, _imgface );

		gfx->color().set( 1.0f, 1.0f, 1.0f, 1.0f );
		gfx->drawLines( &_pts[ 0 ], _pts.size() );
		for( int i = 0; i < _ptsi.size(); i++ ) {
			char buf[ 50 ];
			sprintf( buf, "%d", i );
			gfx->drawText( _ptsi[ i ].x, _ptsi[ i ].y, buf );
//			gfx->drawIcons( &_ptsi[ 0 ], _ptsi.size(), GFX::ICON_CROSS );
		}

	};

	private:
		Image _imgface;
		std::vector<Vector2f> _pts;
		std::vector<Vector2i> _ptsi;
};

int main( int argc, char** argv )
{
	if( argc != 3 ) {
		std::cerr << argv[ 0 ] << " pngfile asffile" << std::endl;
		return 1;
	}
	FaceShapeWin win( argv[ 1 ], argv[ 2 ] );
	win.setSize( 640 * SCALE, 480 * SCALE );
	win.show();

	Application::run();
}
