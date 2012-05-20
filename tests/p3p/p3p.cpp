#include <iostream>

#include <cvt/vision/Vision.h>
#include <cvt/geom/PointSet.h>
#include <cvt/math/Math.h>
#include <cvt/math/Vector.h>
#include <cvt/math/Matrix.h>
#include <cvt/util/Time.h>
#include <cvt/math/sac/RANSAC.h>
#include <cvt/math/sac/P3PSac.h>

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
        p.z = Math::rand( (T)10, (T)1000 );

        ptset.add( p );
    }
}

template <typename T>
void addOutlier( PointSet<2, T> & pset, size_t n ){
    for( size_t i = 0; i < n; i++ ){
        pset.add( Vector2<T>(
                      Math::rand( (T)0, (T)640 ),
                      Math::rand( (T)0, (T)480 )
                      ));
    }
}

void standardTest()
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

    genPoints( ptset, 4 );
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
    Vector3d pp;
    Vector2d p2;
    int correctIdx = -1;
    for( size_t i = 0; i < estimated.size(); i++ ){
        std::cout << "Estimated[ " << i << ":\n" << estimated[ i ] << std::endl;
        // reproject 4th point
        pp = estimated[ i ] * ptset[ 2 ];
        p2 = K * pp;
        double error = ( p2 - ptset2d[ 2 ] ).length();
        std::cout << error << std::endl;
        if( error < 0.01 ){
            correctIdx = i;
        }
    }
    std::cout << "Correct: " << correctIdx << std::endl;
}

void ransacTest()
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

    genPoints( ptset, 80 );
    transformPoints( ptset2d, K, R, t, ptset, 0.0 );

    // add outliers:
    genPoints( ptset, 10 );
    addOutlier( ptset2d, 10 );

    Matrix3d kinv = K.inverse();

    Matrix4d estimated;
    Time time;
    P3PSac model( ptset, ptset2d, K, kinv );
    RANSAC<P3PSac> ransac( model, 1.0, 0.1 );
    estimated = ransac.estimate( 1000 );

    std::cout << "speed: " << time.elapsedMilliSeconds() << "ms" << std::endl;


    std::cout << "TRUE:\n" << R << " t: " << t << std::endl;
    std::cout << "Estimated:\n" << estimated << std::endl;
}



int main( void )
{

    //standardTest();
    ransacTest();

    return 0;
}
