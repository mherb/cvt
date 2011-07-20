/*
 * File:   ransac.cpp
 * Author: sebi, heise
 *
 * Created on July 19, 2011, 4:51 PM
 */

#include <cvt/math/sac/RANSAC.h>
#include <cvt/math/sac/Line2DSAC.h>
#include <cvt/math/sac/HomographySAC.h>

#include <cvt/math/Matrix.h>
#include <cvt/math/Vector.h>

#include <cvt/vision/FeatureMatch.h>

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
    std::vector<Feature2Df> points0;
    std::vector<Feature2Df> points1;
    std::vector<FeatureMatch> matches;

    Matrix3f H;
    

    generateRandomFeatures( points0, 1000 );
    transformPoints( points1, points0, H );

    FeatureMatch m;
    for( size_t i = 0; i < points0.size(); i++ ){
        m.feature0 = &points0[ i ];
        m.feature1 = &points1[ i ];
        matches.push_back( m );
    }

    HomographySAC sacModel( matches );
    float maxDist = 2.0f;
    float outlierProb = 0.2f;
    RANSAC<HomographySAC> ransac( sacModel, maxDist, outlierProb );

    Matrix3f Hest = ransac.estimate();
}

int main()
{
    srandom( time( NULL ) );

    testLineEstimation();


    return 0;
}
