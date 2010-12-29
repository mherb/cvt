#include <cvt/geom/PointSet.h>
#include <cvt/math/Vector.h>

using namespace cvt;

int main()
{
	PointSet3f ptset[ 2 ];
	Vector3f x( 1.0f, 0.0f, 0.0f );
	ptset[ 0 ].add( x );
	ptset[ 1 ].add( x );
	Matrix4f mat;
	mat = ptset[ 0 ].alignSimilarity( ptset[ 1 ] );
	std::cout << mat << std::endl;
}
