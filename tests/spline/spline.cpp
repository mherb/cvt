#include <cvt/math/Spline2.h>

using namespace cvt;

int main()
{
 Spline2f spl( 0.0f, 0.0f, 50.0f, 20.0f, -10.0f, 20.0f, 40.0f, 0.0f );
 Spline2f spl1, spl2;
 Vector2f pt, norm;
 float t;

/* for( float t = 0; t <= 1.0f; t += 0.01f ) {
	 spl.samplePoint( pt, t );
	 std::cout << pt.x << " " << pt.y << std::endl;
 } */

/* spl.split( spl1, spl2, 0.5f );
 for( float t = 0; t <= 1.0f; t += 0.01f ) {
	 spl1.samplePoint( pt, t );
	 std::cout << pt.x << " " << pt.y << std::endl;
	 spl2.samplePoint( pt, t );
	 std::cout << pt.x << " " << pt.y << std::endl;
 }*/

 /*
 size_t n;
 float t[ 2 ];
 n = spl.inflectionPoints( t );
 std::cout << n << " : " << t[ 0 ] << " " << t[ 1 ] << std::endl;
 */

#define DIST 5.0f

    spl.samplePoint( pt, 0.0f );
	spl.sampleDerivative( norm, 0.0f );
	norm.normalize();
    pt.x += norm.y * DIST;
    pt.y += -norm.x * DIST;
	std::cout << pt.x << " " << pt.y << std::endl;

    do {
		t = spl.flattenOffset( DIST, 0.01f );
		spl.samplePoint( pt, 0.0f );
		spl.sampleDerivative( norm, 0.0f );
		norm.normalize();
	    pt.x += norm.y * DIST;
		pt.y += -norm.x * DIST;
		std::cout << pt.x << " " << pt.y << std::endl;
		if( t < 1e-2f)
			t = 1e-2f;
        spl.split( spl1, spl2, t );
		spl = spl2;
    } while(t < 1.0f);

	spl.set( 0.0f, 0.0f, 50.0f, 20.0f, -10.0f, 20.0f, 40.0f, 0.0f );
	spl.samplePoint( pt, 1.0f );
	spl.sampleDerivative( norm, 1.0f );
	norm.normalize();
    pt.x += norm.y * DIST;
    pt.y += -norm.x * DIST;
	std::cout << pt.x << " " << pt.y << std::endl;


}
