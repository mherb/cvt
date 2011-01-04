#include <cvt/geom/PointSet.h>
#include <cvt/math/Vector.h>
#include <cvt/math/Quaternion.h>

using namespace cvt;

int main()
{
	PointSet3f ptset;

	srand( time( NULL ) );

	for( int i = 0; i < 10; i++ )
		ptset.add( Vector3f( Math::rand( -100.0f, 100.0f), Math::rand( -100.0f, 100.0f), Math::rand( -100.0f, 100.0f) ) );

	std::cout << "Initial " << ptset << std::endl;

	Matrix4f m, ms;
	m.identity();
//	m[ 3 ][ 3 ] = 1.0f;
	m[ 0 ][ 3 ] = 3.0f;
	m[ 1 ][ 3 ] = 5.0f;
	m[ 2 ][ 3 ] = -2.0f;

	Quaternionf qrot;
	qrot.setRotation( 1.0f, 0.0f, 0.0f, Math::rand( -1.0f, 1.0f ) );
	m *= qrot.toMatrix4();

	PointSet3f ptset2( ptset );
	ptset2.transform( m );

	std::cout << "Transformed " << ptset2 << std::endl;

	std::cout << "Transform\n" << m << std::endl;
	ms = ptset.alignSimilarity( ptset2 );
	std::cout << "Transform estimated\n" << ( ms) << std::endl;

	std::cout << "Diff\n" << (m - ms) << std::endl;
}
