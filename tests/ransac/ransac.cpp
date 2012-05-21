/*
 * File:   ransac.cpp
 * Author: sebi, heise
 *
 * Created on July 19, 2011, 4:51 PM
 */

#include <cvt/math/sac/RANSAC.h>
#include <cvt/math/sac/Line2DSAC.h>
#include <cvt/math/sac/HomographySAC.h>
#include <cvt/math/sac/EssentialSAC.h>
#include <cvt/math/sac/EPnPSAC.h>

#include <cvt/math/Matrix.h>
#include <cvt/math/Vector.h>

#include <cvt/vision/FeatureMatch.h>

#include <cvt/util/Time.h>

#include <vector>

using namespace cvt;

void generatePointsOnLine( std::vector<Vector2f> & pts, size_t n, size_t numout, float m, float t )
{
    while( n-- ){
        pts.push_back( Vector2f() );
        pts.back().x = Math::rand( -10000.0f, 10000.0f );
        pts.back().y = m * pts.back().x + t;
    }

    while( numout-- ){
        pts.push_back( Vector2f( Math::rand( -10000.0f, 10000.0f ), Math::rand( -10000.0f, 10000.0f ) ) );
    }
}

void generateRandomFeatures( std::vector<Feature2Df> & pts, size_t num )
{
    float x, y;
    while( num-- ){
        x = Math::rand( -1000.0f, 1000.0f );
        y = Math::rand( -1000.0f, 1000.0f );

        pts.push_back( Feature2Df( x, y ) );
    }
}

void addOutlierMatches( std::vector<Feature2Df> & features,
                        std::vector<FeatureMatch> & matches, size_t num )
{
    features.clear();
    generateRandomFeatures( features, 1000 );
    FeatureMatch m;
    while( num-- ){
        // select two indices:
        size_t idx0 = Math::rand( 0, features.size() - 1 );
        size_t idx1 = Math::rand( 0, features.size() - 1 );

        m.feature0 = &features[ idx0 ];
        m.feature1 = &features[ idx1 ];

        matches.push_back( m );
    }
}

void transformPoints( std::vector<Feature2Df> & transformed, const std::vector<Feature2Df> & pts, const Matrix3f & H )
{
    transformed.reserve( pts.size() );
    Vector2f pp;
    for( size_t i = 0; i < pts.size(); i++ ){
        pp = H * pts[ i ].pt;
        transformed.push_back( Feature2Df( pp.x, pp.y ) );
    }
}

void testLineEstimation()
{
    std::cout << "Line Estimation:" << std::endl;
    std::vector<Vector2f> points;

    float outlierProb = 0.1f;
    generatePointsOnLine( points, 1000, 1000 * outlierProb,  0.5f, 10.0f );

    Line2Df l( Vector2f( 0, 10 ), Vector2f( 10, 15 ) );
    std::cout << "Ground Truth:\t" << l.vector() << std::endl;

    Line2DSAC sacModel( points );
    RANSAC<Line2DSAC> ransac( sacModel, 2.0f, outlierProb );

    Line2Df line = ransac.estimate();

    std::cout << "Estimated:\t" << line.vector() << std::endl;
    std::cout << std::endl;
}

void testHomography()
{
	std::cout << "HOMOGRAPHY TEST" << std::endl;
    std::vector<Feature2Df> points0;
    std::vector<Feature2Df> points1;
    std::vector<FeatureMatch> matches;

    Matrix3f H;

    H.setHomography( Math::deg2Rad( 10.0f ), Math::deg2Rad( 5.0f ), 1.05f, 0.99f, 10.0f, -20.0f, 0.000001f, 0.000001f );

    generateRandomFeatures( points0, 1000 );
    transformPoints( points1, points0, H );

    std::vector<Feature2Df> outliers;
    addOutlierMatches( outliers, matches, 200 );

    FeatureMatch m;
    for( size_t i = 0; i < points0.size(); i++ ){
        m.feature0 = &points0[ i ];
        m.feature1 = &points1[ i ];
        matches.push_back( m );
    }

    HomographySAC sacModel( matches );
    float maxDist = 4.0f;
    float outlierProb = 0.16f;
    RANSAC<HomographySAC> ransac( sacModel, maxDist, outlierProb );

    Matrix3f Hest = ransac.estimate();

    std::cout << "Homography: " << std::endl;
    std::cout << "GT:\n" << H << std::endl;
    std::cout << "Estimated:\n" << Hest << std::endl;
}

void genPointSet3d( PointSet3d & pSet, size_t num )
{
	Vector3d p;
	while( num-- ){
		p.x = Math::rand( -1000.0, 1000.0 );
		p.y = Math::rand( -1000.0, 1000.0 );
		p.z = Math::rand( -1000.0, 1000.0 );
		pSet.add( p );
	}
}

void projectPointSet( PointSet2d & p2d, const PointSet3d & p3d, const Matrix4d & world2Cam, const Matrix3d & K )
{
	Matrix3d R = world2Cam.toMatrix3();
	Vector3d t( world2Cam[ 0 ][ 3 ], world2Cam[ 1 ][ 3 ], world2Cam[ 2 ][ 3 ] );
	R.transposeSelf();
	R = K * R;
	t = -R * t;

	Vector3d p3;
	Vector2d p2;
	
	for( size_t i = 0; i < p3d.size(); i++ ){
		p3 = R * p3d[ i ] + t;
		p2.x = p3.x / p3.z;
		p2.y = p3.y / p3.z;

        p2.x += Math::rand( -1.0, 1.0 );
        p2.y += Math::rand( -1.0, 1.0 );

		p2d.add( p2 );
	}
}

void testEPnP()
{
	std::cout << "EPnP TEST" << std::endl;
    PointSet3d points3d;
    PointSet2d points2d;

    Matrix4d transform;
	transform.setRotationXYZ( Math::deg2Rad( Math::rand( -10.0, 10.0 ) ), 
							  Math::deg2Rad( Math::rand( -10.0, 10.0 ) ),
							  Math::deg2Rad( Math::rand( -10.0, 10.0 ) ) );
	transform.setTranslation( Math::rand( -100.0, 100.0 ), 
							  Math::rand( -100.0, 100.0 ),
							  Math::rand( -100.0, 100.0 ) );

	Matrix3d intrinsics;
	intrinsics.setIdentity();
	intrinsics[ 0 ][ 0 ] = 650.0;
	intrinsics[ 0 ][ 2 ] = 320.1;
	intrinsics[ 1 ][ 1 ] = 651.0;
	intrinsics[ 1 ][ 2 ] = 240.4;

    genPointSet3d( points3d, 500 );
	projectPointSet( points2d, points3d, transform, intrinsics );

    EPnPSAC sacModel( points3d, points2d, intrinsics );
    float maxDist = 4.0f;
    float outlierProb = 0.05f;
    RANSAC<EPnPSAC> ransac( sacModel, maxDist, outlierProb );

	Time t;
    Matrix4d est = ransac.estimate( 10000 );
	std::cout << "RANSAC took: " << t.elapsedMilliSeconds() << "ms" << std::endl;

    std::cout << "Transform: " << std::endl;
    std::cout << "GT:\n" << transform.inverse() << std::endl;
    std::cout << "Estimated:\n" << est << std::endl;
}


template <typename T>
static inline void generate3dPoints( std::vector<Vector3<T> > & pts, size_t n )
{
	Vector3<T> p;
	srandom( time( NULL ) );
	while( n-- ){
		p.x = Math::rand( ( T )-1000, ( T )1000 );
		p.y = Math::rand( ( T )-1000, ( T )1000 );
		p.z = Math::rand( ( T )-1000, ( T )1000 );
		pts.push_back( p );
	}
}

static inline void computeEssentialFeatures( std::vector<FeatureMatch>	 & matches,
											 std::vector<Feature2Df>     & points0,
											 std::vector<Feature2Df>     & points1,
											 const std::vector<Vector3f> & points3d,
										     const Matrix3f				 & K,
											 const Matrix4f				 & t01,
											 size_t numOutliers )
{
	Vector2f p, pp;
	Vector3f kt;

	Vector3f t( t01[ 0 ][ 3 ], t01[ 1 ][ 3 ], t01[ 2 ][ 3 ] );
	kt = K * t;

	FeatureMatch m;

	Feature2Df ft;

	points0.reserve( points3d.size() + numOutliers ); 
	points1.reserve( points3d.size() + numOutliers ); 

	for( size_t i = 0; i < points3d.size(); i++ ){
		ft.pt = Vector2f( K * points3d[ i ] );
		points0.push_back( ft );

		ft.pt = Vector2f( K * t01.toMatrix3() * points3d[ i ] + kt );
		points1.push_back( ft );

		m.feature0 = &points0.back();
		m.feature1 = &points1.back();
	    m.distance = 100;
		matches.push_back( m );	
	}

	while( numOutliers-- ){
		ft.pt.x = Math::rand( 0.0f, 640.0f );
		ft.pt.y = Math::rand( 0.0f, 480.0f );
		points0.push_back( ft );
		ft.pt.x = Math::rand( 0.0f, 640.0f );
		ft.pt.y = Math::rand( 0.0f, 480.0f );
		points1.push_back( ft );

		m.feature0 = &points0.back();
		m.feature1 = &points1.back();
	    m.distance = 100;
		matches.push_back( m );	
	}
}

void testEssential()
{
	std::cout << "ESSENTIAL TEST" << std::endl;
    std::vector<Feature2Df>		points0;
    std::vector<Feature2Df>		points1;
    std::vector<FeatureMatch>	matches;
	std::vector<Vector3f>		points3d;

	Matrix4<float> trans;
	trans.setIdentity();

	Vector3<float> t( 10.0f, 30.0f, 50.0f );
	trans.setRotationXYZ( Math::deg2Rad<float>( 20 ), Math::deg2Rad<float>( 10 ), Math::deg2Rad<float>( 30 ) );
	trans.setTranslation( t.x, t.y, t.z );

	Matrix3<float> K( 500.0,   0.0, 320.0,
					  0.0,   505.0, 240.0,
				  	  0.0,     0.0,   1.0 );

	size_t numfeature = 300;
	size_t outliers = 30;
	generate3dPoints( points3d, numfeature );

	computeEssentialFeatures( matches, points0, points1, points3d, K, trans, outliers );

    EssentialSAC sacModel( matches, K );

    float maxDist = 0.5f;
    float outlierProb = (float)( outliers ) / ( float )( outliers + numfeature );
    RANSAC<EssentialSAC> ransac( sacModel, maxDist, outlierProb );

    Matrix3f essential = ransac.estimate();
	essential *= 1.0f / essential[ 2 ][ 2 ];

	// essential matrix = [t]_x R
	Matrix3<float> skew, Etrue;
	skew.setSkewSymmetric( t );
	Etrue = skew * trans.toMatrix3();
	Etrue *= 1.0 / Etrue[ 2 ][ 2 ];

    std::cout << "Essential: " << std::endl;
    std::cout << "GT:\n" << Etrue << std::endl;
    std::cout << "Estimated:\n" << essential << std::endl;

}

int main()
{
    srandom( time( NULL ) );

//    testLineEstimation();
//    testHomography();
//    testEssential();
	testEPnP();

    return 0;
}

