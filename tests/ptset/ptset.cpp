#include <cvt/geom/PointSet.h>
#include <cvt/math/Vector.h>
#include <cvt/math/Quaternion.h>

using namespace cvt;

int main()
{
	PointSet3f ptset;

	for( int i = 0; i < 10; i++ )
		ptset.add( Vector3f( Math::rand( -1.0f, 1.0f), Math::rand( -1.0f, 1.0f), Math::rand( -1.0f, 1.0f) ) );

	std::cout << "Initial " << ptset << std::endl;

	Matrix4f m, ms;
	m.identity();
//	m[ 0 ][ 3 ] = 3.0f;
//	m[ 1 ][ 3 ] = 5.0f;
//	m[ 2 ][ 3 ] = 2.0f;

	Quaternionf qrot( 1.0f, 0.0f, 0.0f, 0.5f );
//	m *= qrot.toMatrix4();

	PointSet3f ptset2( ptset );
	ptset2.transform( m );

	std::cout << "Transform\n" << m << std::endl;
	ms = ptset.alignSimilarity( ptset2 );
	std::cout << "Transform estimated\n" << ( ms) << std::endl;
	std::cout << "Transformed both\n" << ( ms * m ) << std::endl;
}
