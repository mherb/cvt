#include <cvt/geom/PointSet.h>
#include <cvt/math/Vector.h>

using namespace cvt;

int main()
{
	PointSet3f ptset;

	ptset.add( Vector3f( 0.0f, 1.0f, 0.0f ) );
	ptset.add( Vector3f( 1.0f, 0.0f, 0.0f ) );
	ptset.add( Vector3f( 0.0f, 0.0f, 1.0f ) );

	std::cout << "Initial " << ptset << std::endl;
	ptset.scale( 3.0f );
	std::cout << "After scale: " << ptset << std::endl;

	Matrix4f m;
	m.identity();
	m[ 1 ][ 3 ] = 1.0f;
	ptset.transform( m );
	std::cout << "After transform: " << ptset << std::endl;
	std::cout << "Mean: " << ptset.mean() << std::endl;
}
