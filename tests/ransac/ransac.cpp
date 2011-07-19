/*
 * File:   ransac.cpp
 * Author: sebi, heise
 *
 * Created on July 19, 2011, 4:51 PM
 */

#include <cvt/math/sac/RANSAC.h>
#include <cvt/math/sac/Line2DSAC.h>

#include <cvt/math/Vector.h>

using namespace cvt;

void generatePoints( std::vector<Vector2f> & pts, size_t n, size_t numout, float m, float t )
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

int main()
{
    srandom( time( NULL ) );
    std::vector<Vector2f> points;

    float outlierProb = 0.1f;
    generatePoints( points, 1000, 1000 * outlierProb,  0.5f, 10.0f );

    Line2Df l( Vector2f( 0, 10 ), Vector2f( 10, 15 ) );
    std::cout << l.vector() << std::endl;

    Line2DSAC sacModel( points );
    RANSAC<Line2DSAC> ransac( sacModel, 2.0f, outlierProb );

    Line2Df line = ransac.estimate();

    std::cout << line.vector() << std::endl;

    return 0;
}

