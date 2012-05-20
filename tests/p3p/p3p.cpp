#include <iostream>

#include <cvt/vision/Vision.h>
#include <cvt/geom/PointSet.h>
#include <cvt/math/Math.h>
#include <cvt/math/Vector.h>
#include <cvt/math/Matrix.h>
#include <cvt/util/Time.h>

using namespace cvt;

template <typename T>
void transformPoints( PointSet<2, T> & result, const Matrix3<T> & K, const Matrix3<T> & R, const Vector3<T>& t, const PointSet<3, T> & pts3d, T noise )
{
    Matrix3<T> KR = K * R;
    Vector3<T> Kt = K * t;
    Vector3<T> point;
    Vector2<T> p;

    for( size_t i = 0; i < pts3d.size(); i++ ){
        point = KR * pts3d[ i ] + Kt;

        p.x = point.x / point.z;
        p.y = point.y / point.z;

        //p.x += Math::rand( -noise, noise );
        //p.y += Math::rand( -noise, noise );

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
        p.z = Math::rand( (T)10, (T)1000 );

        ptset.add( p );
    }
}



int main( void )
{
    PointSet<3, double> ptset;
    PointSet<2, double> ptset2d;

    Matrix3d R; Vector3d t;
    Matrix3<double> K;

    R.setRotationXYZ( 0.1, 0.0, 0.0 );
    t.set( 0, 0, 10 );

    K.setZero();
    K[ 0 ][ 0 ] = 650.0; K[ 0 ][ 2 ] = 320.0;
    K[ 1 ][ 1 ] = 650.0; K[ 1 ][ 2 ] = 240.0;
    K[ 2 ][ 2 ] = 1;

    genPoints( ptset, 3 );
    transformPoints( ptset2d, K, R, t, ptset, 0.0 );

    std::vector<Vector3d> featureVecs;
    Matrix3d kinv = K.inverse();

    featureVecs.resize( ptset2d.size() );
    Vector3d tmp;
    for( size_t i = 0; i < ptset2d.size(); i++ ){
        tmp.x = ptset2d[ i ].x;
        tmp.y = ptset2d[ i ].y;
        tmp.z = 1.0;

        featureVecs[ i ] = kinv * tmp;
        featureVecs[ i ].normalize();
        std::cout << "point: " << ptset2d[ i ] << " \t->\t vec: " << featureVecs[ i ] << std::endl;
    }

    std::vector<Matrix4d> estimated;    
    Time time;

    Vision::p3p( estimated, &featureVecs[ 0 ], ( Vector3d* )ptset.ptr() );

    //p3p( estimated, (Vector3d*)ptset.ptr(), &featureVecs[ 0 ] );
    std::cout << "speed: " << time.elapsedMilliSeconds() << "ms" << std::endl;

    std::cout << "TRUE:\n" << R << " t: " << t << std::endl;
    for( size_t i = 0; i < estimated.size(); i++ ){
        std::cout << "Estimated[ " << i << ":\n" << estimated[ i ] << std::endl;
    }

    return 0;
}
