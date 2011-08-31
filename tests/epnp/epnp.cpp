#include <iostream>

#include <cvt/vision/EPnP.h>
#include <cvt/geom/PointSet.h>
#include <cvt/math/Math.h>
#include <cvt/math/Vector.h>
#include <cvt/math/Matrix.h>
#include <cvt/util/Time.h>


using namespace cvt;

template <typename T>
void transformPoints( PointSet<2, T> & result, const Matrix4<T> & transform, const Matrix4<T> & K, const PointSet<3, T> & pts3d, T noise )
{
	Matrix4<T> P = K * transform;
	Vector3<T> point;
	Vector2<T> p;

	for( size_t i = 0; i < pts3d.size(); i++ ){
		point = P * pts3d[ i ];

	//	std::cout << "Transformed point: " << point << std::endl;

		p.x = point.x / point.z;
		p.y = point.y / point.z;

		p.x += Math::rand( -noise, noise );
		p.y += Math::rand( -noise, noise );

		result.add( p );
	}
}

template <typename T>
void genPoints( PointSet<3, T> & ptset, size_t n )
{

	srandom( time( NULL ) );
	Vector3<T> p;
	while( n-- ){
		p.x = Math::rand( (T)-1000, (T)1000 );
		p.y = Math::rand( (T)-1000, (T)1000 );
		p.z = Math::rand( (T)-1000, (T)1000 );

		ptset.add( p );
	}

/*	
    ptset.add( Vector3<T>( (T)100, (T)200, (T)300 ) );
    ptset.add( Vector3<T>( (T)-100, (T)200, (T)400 ) );
    ptset.add( Vector3<T>( (T)130, (T)220, (T)400 ) );
    ptset.add( Vector3<T>( (T)10, (T)20, (T)60 ) );
    ptset.add( Vector3<T>( (T)-50, (T)40, (T)80 ) );
    ptset.add( Vector3<T>( (T)310, (T)20, (T)90 ) );
    ptset.add( Vector3<T>( (T)210, (T)60, (T)200 ) );
    ptset.add( Vector3<T>( (T)-61, (T)-20, (T)600 ) );
    ptset.add( Vector3<T>( (T)80, (T)-50, (T)70 ) );
    ptset.add( Vector3<T>( (T)10, (T)-168, (T)60 ) );
*/	
}

int main( void )
{
	PointSet<3, double> ptset;
	PointSet<2, double> ptset2d;

	Matrix4<double> transform, K44;
	Matrix3<double> K;

	transform.setRotationXYZ( 0.1, 0.2, 0.4 );
	transform.setTranslation( 200, 211, 100 );

	K.setZero();
	K[ 0 ][ 0 ] = 650.5; K[ 0 ][ 2 ] = 320.0;
	K[ 1 ][ 1 ] = 650.0; K[ 1 ][ 2 ] = 240.0;
    K[ 2 ][ 2 ] = 1;

	K44.setZero();
	for( size_t i = 0; i < 3; i++ )
		for( size_t k = 0; k < 3; k++ )
			K44[ i ][ k ] = K[ i ][ k ];
	K44[ 3 ][ 3 ] = 1;

	genPoints( ptset, 100 );
	transformPoints( ptset2d, transform, K44, ptset, 5.0 );

	Time t;
	EPnP<double> epnp( ptset );

	Matrix4<double> estimated;
	epnp.solve( estimated, ptset2d, K );

	std::cout << "EPnP time: " << t.elapsedMilliSeconds() << "ms" << std::endl;

	std::cout << "TRUE:\n" << transform << std::endl;
	std::cout << "Estimated:\n" << estimated << std::endl;

	return 0;
}
