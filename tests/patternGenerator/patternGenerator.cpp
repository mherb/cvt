#include <iostream>
#include <set>

#include <cvt/math/Vector.h>
#include <cvt/math/Matrix.h>
#include <cvt/math/Math.h>

#include "orb_orig_pattern.h"

using namespace cvt;

struct Test
{
    Vector2f    p0;
    Vector2f    p1;
};

namespace std {
    template<> bool less<Vector2f>::operator()( const Vector2f & a, const Vector2f & b ) const 
    {
        if( ( a.x < b.x ) || ( a.x == b.x && a.y < b.y ) )
            return true;
        return false;
    }
}

void genFixedCircularPositions( std::set<Vector2f> & positions,
							    size_t numCircles,
								size_t testRadius,
								size_t patchRadius )
{
	positions.insert( Vector2f( 0, 0 ) );
    // TODO
}

void originalOrbTests( std::vector<Test> & tests )
{
    tests.resize( 256 );
    std::vector<Test>::iterator it = tests.begin();
    for( size_t i = 0; i < 256; i++ ){
        it->p0.x = orbpattern[ 4 * i + 0 ];        
        it->p0.y = orbpattern[ 4 * i + 1 ];
        it->p1.x = orbpattern[ 4 * i + 2 ];        
        it->p1.y = orbpattern[ 4 * i + 3 ];
        it++;
    }    
}

void rotateAndDumpPattern( const std::vector<Test> & tests, size_t numRotations )
{
    Matrix3f rot;
    Vector2f p0, p1;
    float rad = 0;
    float inc = Math::TWO_PI / ( float )numRotations;
    std::cout << "const float ORB::_patterns[ " <<  numRotations << " ][ " << tests.size() * 2 << " ][ 2 ] = \n{\n";
    
    for( size_t r = 0; r < numRotations; r++, rad+=inc ){
        rot.setRotationZ( rad );
        std::cout << "\t{\n";
        for( size_t i = 0; i < tests.size(); i++ ){
            p0 = rot * tests[ i ].p0;
            p1 = rot * tests[ i ].p1;
            std::cout << "\t\t{ " <<  p0.x << ", " << p0.y <<" }, ";
			std::cout << "{ " <<  p1.x << ", " << p1.y <<" },\n";
        }
        std::cout << "\t},\n";
    }
    std::cout << "};\n";
}

int main()
{
	std::set<Vector2f> uniquePositions;
    std::vector<Test> tests;

//	genCircularPositions( uniquePositions, 3, 5, 15 );    
    originalOrbTests( tests );    
    rotateAndDumpPattern( tests, 30 );
    
	return 0;
}
