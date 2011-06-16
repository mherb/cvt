#include <cvt/math/Math.h>
#include <cvt/math/Matrix.h>
#include <cvt/math/Vector.h>

#include "orb_pattern.hpp"

using namespace cvt;

int main()
{
	float inc = Math::TWO_PI / 30.0f;
	float rad = 0;
//	std::cout << "Vector2i ORB::_patterns[ 30 ][ 256 ][ 2 ]\n{\n";
	for( int i = 0; i < 30; i++, rad += inc ) {
		Matrix3f rot;
		rot.rotationZ( rad );
//		std::cout << "\t{\n";
		for( int k = 0; k < 256; k++ ) {
			Vector2f p1, p2;
			p1.set( orbpattern[ k * 4 + 0] + 0.5f,  orbpattern[ k * 4 + 1] + 0.5f );
			p2.set( orbpattern[ k * 4 + 2] + 0.5f,  orbpattern[ k * 4 + 3] + 0.5f );

			p1 = rot * p1;
			p2 = rot * p2;

			if( Math::abs( ( int )p1.x ) > 13 )
				std::cout << "ERROR " << p1.x << std::endl;
			if( Math::abs( ( int )p1.y ) > 13 )
				std::cout << "ERROR " << p1.y << std::endl;
			if( Math::abs( ( int )p2.x ) > 13 )
				std::cout << "ERROR "  << p2.x << std::endl;
			if( Math::abs( ( int )p2.y ) > 13 )
				std::cout << "ERROR " << p2.y << std::endl;

//			std::cout << "\t\t{ " <<  ( ( int ) p1.x )  << ", " << ( ( int ) p1.y ) <<" }, ";
//			std::cout << "{ " <<  ( ( int ) p2.x )  << ", " << ( ( int ) p2.y ) <<" },\n";
		}
//		std::cout << "\t},\n";
	}
//	std::cout << "};\n";
}
