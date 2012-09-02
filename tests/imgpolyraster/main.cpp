#include <cvt/gfx/Image.h>
#include <cvt/geom/Polygon.h>
#include <cvt/geom/Rect.h>

#include "IPolygonRaster.h"

using namespace cvt;


int main()
{
	Image output( 800, 600, IFormat::RGBA_UINT8 );
	output.fill( Color::BLACK );

	Polygonf poly;
#if 0
	poly.addPoint( Vector2f( 20, 20 ) );
	poly.addPoint( Vector2f( 150, 120 ) );
	poly.addPoint( Vector2f( 20, 120 ) );
#else
	poly.addPoint( Vector2f( 120, 20 ) );
	poly.addPoint( Vector2f( 160, 80 ) );
	poly.addPoint( Vector2f( 100, 40 ) );
	poly.addPoint( Vector2f( 140, 40 ) );
	poly.addPoint( Vector2f( 80, 80 ) );
//	poly.addPoint( Vector2f( 120, 20 ) );
#endif

	IPolygonRaster raster( poly, Rectf( 0, 0, output.width(), output.height() ) );
	raster.rasterize( output, Color::WHITE );
	output.save( "bla.png" );

	return 0;
}
