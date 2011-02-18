#include <cvt/geom/Line2D.h>

using namespace cvt;

int main()
{
	Line2Df l1( 0, 0, 10, 0 );
	Line2Df l2( 5, 5, 5, -5 );
	Vector2f pt;

	l1.intersect( l2, pt );
	std::cout << pt << std::endl;

	std::cout << l1.distance( pt ) << std::endl;

	pt.set( 5, 5 );
	std::cout << l1.distance( pt ) << std::endl;

	return 0;
}
