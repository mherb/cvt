#include <cvt/math/Math.h>
#include <cvt/math/Matrix.h>
#include <cvt/math/Vector.h>
#include <cvt/gfx/Image.h>
#include <cvt/gfx/GFX.h>
#include <cvt/gfx/GFXEngineImage.h>

#include "orb_pattern.hpp"

using namespace cvt;

int main()
{

#define ROTSIZE 30
#define SCALESIZE 40
#define SCALE 2.0f

	float inc = Math::TWO_PI / ( float ) ROTSIZE;
	float rad = 0;
	std::cout << "const int ORB::_patterns[ " <<  ROTSIZE << " ][ " << SCALESIZE << " ][ 512 ][ 2 ] = \n{\n";
	for( int i = 0; i < ROTSIZE; i++, rad += inc ) {
		Matrix2f rot, T;
		rot.setRotationZ( rad );
		std::cout << "\t{\n";
		for( int s = 0; s < SCALESIZE; s++ ) {
			std::cout << "\t{\n";
			T = rot * Matrix2f( SCALE, 0.0f, 0.0f, ( float ) s * SCALE / ( float ) SCALESIZE );
			for( int k = 0; k < 256; k++ ) {
				Vector2f p1, p2;
				p1.set( orbpattern[ k * 4 + 0] + 0.0f,  orbpattern[ k * 4 + 1] + 0.0f );
				p2.set( orbpattern[ k * 4 + 2] + 0.0f,  orbpattern[ k * 4 + 3] + 0.0f );

				p1 = T * p1;
				p2 = T * p2;

				std::cout << "\t\t{ " <<  ( ( int ) Math::round( p1.x ) )  << ", " << ( ( int ) Math::round( p1.y ) ) <<" }, ";
				std::cout << "{ " <<  ( ( int ) Math::round( p2.x ) )  << ", " << ( ( int ) Math::round( p2.y ) ) <<" },\n";
			}
			std::cout << "\t},\n";
		}
		std::cout << "\t},\n";
	}
	std::cout << "};\n";
}
