#include <iostream>

#include <cvt/vision/Vision.h>
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

void p3p( std::vector<Matrix4d>& transforms, const Vector3d* pw, const Vector3d* featureVec )
{
    // TODO: we assume that we this is called with exactly 3 point correspondences!
    Vector3d P1 = pw[ 0 ];
    Vector3d P2 = pw[ 1 ];
    Vector3d P3 = pw[ 2 ];

    // if the points are colinear, the algorithm does not work!
    Vector3d temp1 = P2 - P1;
    Vector3d temp2 = P3 - P1;
    if( temp1.cross( temp2 ).length() == 0 )
        return;

    const Vector3d& f1 = featureVec[ 0 ];
    const Vector3d& f2 = featureVec[ 1 ];
    const Vector3d& f3 = featureVec[ 2 ];

    // intermediate camera frame
    Vector3d e1 = f1;
    e1.normalize();
    Vector3d e3 = e1.cross( f2 );
    e3.normalize();
    Vector3d e2 = e3.cross( e1 );

    Matrix3d T( e1, e2, e3 );

    Vector3d f3v = T * f3;
    Vector3d f1v, f2v;

    f1v = featureVec[ 1 ];
    f2v = featureVec[ 0 ];
    f3v = featureVec[ 2 ];
    f1v.normalize();
    f2v.normalize();
    f3v.normalize();

    // Reinforce that f3[ 2 ] > 0 for having theta in [0;pi]
    if( f3v[ 2 ] > 0 ) {
        f1v = featureVec[ 1 ];
        f2v = featureVec[ 0 ];
        f3v = featureVec[ 2 ];
        f1v.normalize();
        f2v.normalize();
        f3v.normalize();


        e1 = f1v;
        e1.normalize();
        e3 = e1.cross( f2v );
        e3.normalize();
        e2 = e3.cross( e1 );

        T[ 0 ] = e1;
        T[ 1 ] = e2;
        T[ 2 ] = e3;
        f3v = T * f3v;

        P1 = pw[ 1 ];
        P2 = pw[ 0 ];
        P3 = pw[ 2 ];
    }

    //intermediate world frame
    Vector3d p_21 = P2 - P1;
    Vector3d p_31 = P3 - P1;
    Vector3d n1 = p_21;
    n1.normalize();
    double d_12 = p_21.length();


    Vector3d n3 = n1.cross( p_31 );
    n3.normalize();
    Vector3d n2 = n3.cross( n1 );

    Matrix3d N( n1, n2, n3 );

    // Extraction of known parameters
    Vector3d P3n = N * p_31;

    double f_1 = f3v[ 0 ] / f3v[ 2 ];
    double f_2 = f3v[ 1 ] / f3v[ 2 ];
    double p_1 = P3n[ 0 ];
    double p_2 = P3n[ 1 ];

    double cos_beta = f1v * f2v;
    double b = 1.0 / ( 1.0 - Math::pow( cos_beta, 2.0 ) ) - 1.0;

    std::cout << "Cos_beta: " << cos_beta << std::endl;

    if( cos_beta < 0.0 ){
        b = -Math::sqrt( b );
    } else {
        b = Math::sqrt( b );
    }

    // Definition of temporary variables for avoiding multiple computation
    double f_1_2  = Math::sqr( f_1 );
    double f_2_2  = Math::sqr( f_2 );
    double p_1_2  = Math::sqr( p_1);
    double p_1_3  = p_1_2 * p_1;
    double p_1_4  = p_1_3 * p_1;
    double p_2_2  = Math::sqr( p_2 );
    double p_2_3  = p_2_2 * p_2;
    double p_2_4  = p_2_3 * p_2;
    double d_12_2 = Math::sqr( d_12 );
    double b_2    = Math::sqr( b );

    // Computation of factors of 4th degree polynomial
    double factors[ 5 ];

    factors[ 0 ] = -f_2_2 * p_2_4 - p_2_4 * f_1_2 - p_2_4;
    factors[ 1 ] =  2.0 * p_2_3 * d_12 * b + 2.0 * f_2_2 * p_2_3 * d_12 * b - 2.0 * f_2 * p_2_3 * f_1 * d_12;
    factors[ 2 ] =  -f_2_2 * p_2_2 * p_1_2
                    - f_2_2 * p_2_2 * d_12_2 * b_2
                    - f_2_2 * p_2_2 * d_12_2
                    + f_2_2 * p_2_4
                    + p_2_4 * f_1_2
                    + 2.0 * p_1 * p_2_2 * d_12
                    + 2.0 * f_1 * f_2 * p_1 * p_2_2 * d_12 * b
                    - p_2_2 * p_1_2 * f_1_2
                    + 2.0 * p_1 * p_2_2 * f_2_2 * d_12
                    - p_2_2 * d_12_2 * b_2
                    - 2.0 * p_1_2 * p_2_2;

    factors[ 3 ] =   2.0 * p_1_2 * p_2 * d_12 * b
                    +2.0 * f_2 * p_2_3 * f_1 * d_12
                    -2.0 * f_2_2 * p_2_3 * d_12 * b
                    -2.0 * p_1 * p_2 * d_12_2 * b;

    factors[ 4 ] =  -2.0 * f_2 * p_2_2 * f_1 * p_1 * d_12 * b
                    +f_2_2 * p_2_2 * d_12_2
                    +2.0 * p_1_3 * d_12
                    - p_1_2 * d_12_2
                    + f_2_2 * p_2_2 * p_1_2
                    - p_1_4
                    - 2.0 * f_2_2 * p_2_2 * p_1 * d_12
                    + p_2_2 * f_1_2 * p_1_2
                    + f_2_2 * p_2_2 * d_12_2 * b_2;

    std::cout << "Factors: " << factors[ 0 ] << " "
                             << factors[ 1 ] << " "
                             << factors[ 2 ] << " "
                             << factors[ 3 ] << " "
                             << factors[ 4 ] << std::endl;

    // Computation of quartic solutions
    Vector4d realRoots;
    Math::solveQuarticReal( realRoots,
                            factors[ 0 ],
                            factors[ 1 ],
                            factors[ 2 ],
                            factors[ 3 ],
                            factors[ 4 ] );


    // Backsubstitution of each solution
    for(int i=0; i<4; i++) {
        double cot_alpha = ( -f_1 * p_1 / f_2 - realRoots[ i ] * p_2 + d_12 *b ) / ( -f_1 * realRoots[ i ] * p_2 / f_2 + p_1 - d_12 );

        double cos_theta = realRoots[ i ];
        double sin_theta = Math::sqrt( 1.0 - Math::sqr( realRoots[ i ] ) );
        double sin_alpha = Math::sqrt( 1.0 / ( Math::sqr( cot_alpha ) + 1.0 ) );
        double cos_alpha = Math::sqrt( 1.0 - Math::sqr( sin_alpha ) );

        if( cot_alpha < 0.0 )
            cos_alpha = -cos_alpha;

        Vector3d C( d_12 * cos_alpha * ( sin_alpha * b + cos_alpha ),
                    cos_theta * d_12 * sin_alpha * ( sin_alpha * b + cos_alpha ),
                    sin_theta * d_12 * sin_alpha * ( sin_alpha * b + cos_alpha ) );

        C = P1 + N.transpose() * C;

        Matrix3d R;
        R[ 0 ][ 0 ] = -cos_alpha;
        R[ 0 ][ 1 ] = -sin_alpha*cos_theta;
        R[ 0 ][ 2 ] = -sin_alpha*sin_theta;

        R[ 1 ][ 0 ] =  sin_alpha;
        R[ 1 ][ 1 ] = -cos_alpha * cos_theta;
        R[ 1 ][ 2 ] = -cos_alpha * sin_theta;

        R[ 2 ][ 0 ] = 0;
        R[ 2 ][ 1 ] = -sin_theta;
        R[ 2 ][ 2 ] = cos_theta;

        R = N.transpose() * R.transpose() * T;

        transforms.push_back( Matrix4d( R[ 0 ][ 0 ], R[ 0 ][ 1 ], R[ 0 ][ 2 ], C[ 0 ],
                                        R[ 1 ][ 0 ], R[ 1 ][ 1 ], R[ 1 ][ 2 ], C[ 1 ],
                                        R[ 2 ][ 0 ], R[ 2 ][ 1 ], R[ 2 ][ 2 ], C[ 2 ],
                                                0.0,         0.0,         0.0,  1.0 ) );

    }
}

int main( void )
{
    PointSet<3, double> ptset;
    PointSet<2, double> ptset2d;

    Matrix4<double> transform, K44;
    Matrix3<double> K;

    transform.setIdentity();
    //transform.setRotationXYZ( 0.0, 0.0, 0.4 );
    //transform.setTranslation( 10, 21, -100 );

    K.setZero();
    K[ 0 ][ 0 ] = 650.0; K[ 0 ][ 2 ] = 320.0;
    K[ 1 ][ 1 ] = 650.0; K[ 1 ][ 2 ] = 240.0;
    K[ 2 ][ 2 ] = 1;

    K44.setZero();
    for( size_t i = 0; i < 3; i++ )
        for( size_t k = 0; k < 3; k++ )
            K44[ i ][ k ] = K[ i ][ k ];
    K44[ 3 ][ 3 ] = 1;

    //genPoints( ptset, 3 );
    ptset.add( Vector3d( 100, 200, 100 ) );
    ptset.add( Vector3d( 300, 200, 150 ) );
    ptset.add( Vector3d( 150, 250, 200 ) );

    transformPoints( ptset2d, transform, K44, ptset, 0.0 );

    Time t;

    std::vector<Vector3d> featureVecs;
    Matrix3d kinv = K.inverse();
    std::cout << kinv << std::endl;
    featureVecs.resize( ptset2d.size() );
    for( size_t i = 0; i < ptset2d.size(); i++ ){
        featureVecs[ i ].x = ptset2d[ i ].x;
        featureVecs[ i ].y = ptset2d[ i ].y;
        featureVecs[ i ].z = 1.0;
        featureVecs[ i ] = kinv * featureVecs[ i ];
        featureVecs[ i ].normalize();
        std::cout << "vec " << i << ": " << featureVecs[ i ] << std::endl;
    }

    std::vector<Matrix4d> estimated;
    p3p( estimated, (Vector3d*)ptset.ptr(), &featureVecs[ 0 ] );
    std::cout << "speed: " << t.elapsedMilliSeconds() << "ms" << std::endl;

    std::cout << "TRUE:\n" << transform << std::endl;
    for( size_t i = 0; i < estimated.size(); i++ ){
        std::cout << "Estimated[ " << i << ":\n" << estimated[ i ] << std::endl;
    }

    return 0;
}
