#include <cvt/geom/Polygon2D.h>

using namespace cvt;

int main()
{
	Polygon2D<float> poly;

	poly.addPoint( Vector2f( -1.0f, -1.0f ) );
	poly.addPoint( Vector2f( -1.0f,  1.0f ) );
	poly.addPoint( Vector2f(  1.0f, 1.0f ) );
	poly.addPoint( Vector2f(  1.0f, -1.0f ) );
	poly.addPoint( Vector2f( -1.0f, -1.0f ) );

	std::cout << poly.area() << std::endl;
	std::cout << poly.centroid() << std::endl;

}
