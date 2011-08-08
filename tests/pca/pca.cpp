#include <cvt/math/PCA.h>
#include <cvt/math/Vector.h>

using namespace cvt;

int main()
{
	PCAf pca( 2 );

	srand( time( NULL ) );

	Vector2f v1( Math::rand( -1.0f, 1.0f ), Math::rand( -1.0f, 1.0f ) );
	v1.normalize();
	Vector2f v2( -v1[ 1 ], v1[ 0 ] );


	for( int i = 0; i < 100; i++ )
	{
		Vector2f s;
		s.setZero();
		s = Math::rand( -3.0f, 3.0f ) * v1 + Math::rand( -1.0f, 1.0f ) * v2;
		pca.addSample( s.ptr() );
	}

	Eigen::VectorXf m( 2 );
	Eigen::MatrixXf pc( 2, 2 );
	pca.mean( m );
	pca.principleComponents( pc );

	std::cout << v1 << std::endl;
	std::cout << v2 << std::endl;

	std::cout << m << std::endl;
	std::cout << pc << std::endl;

}
