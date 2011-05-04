#include <cvt/geom/Polygon.h>

using namespace cvt;

int main()
{
	Polygon<float> poly;

	poly.addPoint( Vector2f( -1.0f, -1.0f ) );
	poly.addPoint( Vector2f( -1.0f,  1.0f ) );
	poly.addPoint( Vector2f(  1.0f, 1.0f ) );
	poly.addPoint( Vector2f(  1.0f, -1.0f ) );
//  Doesnt' matter if first point is added at the end
//	poly.addPoint( Vector2f( -1.0f, -1.0f ) );

	std::cout << poly.area() << std::endl;
	std::cout << poly.centroid() << std::endl;

}
