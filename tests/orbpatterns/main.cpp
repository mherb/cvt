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

#define SIZE 30

	float inc = Math::TWO_PI / ( float ) SIZE;
	float rad = 0;
	std::cout << "const int ORB::_patterns[ " <<  SIZE << " ][ 512 ][ 2 ] = \n{\n";
	for( int i = 0; i < SIZE; i++, rad += inc ) {
		Image out( 512, 512, IFormat::RGBA_UINT8 );
		GFXEngineImage ge( out );
		GFX g( &ge );
		g.color().set( 1.0f, 1.0f, 1.0f, 1.0f );
		g.fillRect( 0, 0, 512, 512 );
		g.color().set( 0.0f, 1.0f, 0.0f, 1.0f );


		Matrix3f rot;
		rot.rotationZ( rad );
		std::cout << "\t{\n";
		for( int k = 0; k < 256; k++ ) {
			Vector2f p1, p2;
			p1.set( orbpattern[ k * 4 + 0] + 0.5f,  orbpattern[ k * 4 + 1] + 0.5f );
			p2.set( orbpattern[ k * 4 + 2] + 0.5f,  orbpattern[ k * 4 + 3] + 0.5f );

			p1 = rot * p1;
			p2 = rot * p2;

/*			if( Math::abs(p1.x) > 18 || Math::abs( p1.y ) > 18 ) {
				std::cout << p1 << std::endl;
				return 0;
			}
			if( Math::abs(p2.x) > 18 || Math::abs( p2.y ) > 18 ) {
				std::cout << p2 << std::endl;
				return 0;
			}*/

			{
				Vector2f pt1 = p1;
				Vector2f pt2 = p2;
				pt1 *= 10.0f;
				pt2 *= 10.0f;
				pt1 += 256.0f;
				pt2 += 256.0f;
				g.fillRect( ( int ) pt1.x - 1 , ( int ) pt1.y - 1, 3, 3 );
				g.fillRect( ( int ) pt2.x - 1 , ( int ) pt2.y - 1, 3, 3 );
				g.drawLine( ( int ) pt1.x, ( int ) pt1.y,( int ) pt2.x, ( int ) pt2.y );
			}
/*			if( Math::abs( ( int )p1.x ) > 13 )
				std::cout << "ERROR " << p1.x << std::endl;
			if( Math::abs( ( int )p1.y ) > 13 )
				std::cout << "ERROR " << p1.y << std::endl;
			if( Math::abs( ( int )p2.x ) > 13 )
				std::cout << "ERROR "  << p2.x << std::endl;
			if( Math::abs( ( int )p2.y ) > 13 )
				std::cout << "ERROR " << p2.y << std::endl;*/

			std::cout << "\t\t{ " <<  ( ( int ) p1.x )  << ", " << ( ( int ) p1.y ) <<" }, ";
			std::cout << "{ " <<  ( ( int ) p2.x )  << ", " << ( ( int ) p2.y ) <<" },\n";
		}
		String file;
		file.sprintf("pattern-%02d.png", i );
		out.save( file );
		std::cout << "\t},\n";
	}
	std::cout << "};\n";
}
