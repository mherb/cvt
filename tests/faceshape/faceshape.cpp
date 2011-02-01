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


class FaceShapeWin : public Window
{
	public:
	FaceShapeWin( const std::string& pngfile, const std::string& asffile ) : Window( "FaceShape" )
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
			Vector2f x( asfentries[ i ].relx * png.width(), asfentries[ i ].rely * png.height() );
			Vector2i k( asfentries[ i ].relx * ( float ) png.width() - 8, asfentries[ i ].rely * ( float ) png.height() - 8 );
			_pts.push_back( x );
			_ptsi.push_back( k );
			int to = asfentries[ i ].to;
			x.set( asfentries[ to ].relx * png.width(), asfentries[ to ].rely * png.height() );
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

		gfx->drawImage( 0, 0, _imgface );

		gfx->color().set( 1.0f, 1.0f, 1.0f, 1.0f );
		gfx->drawLines( &_pts[ 0 ], _pts.size() );
		gfx->drawIcons( &_ptsi[ 0 ], _ptsi.size(), GFX::ICON_CROSS );

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
	win.setSize( 640, 480 );
	win.show();

	Application::run();
}
