#include <cvt/util/SIMD.h>
#include <cvt/math/Math.h>

#include <stdio.h>

#include <cvt/util/SIMDDebug.h>

using namespace cvt;

int main()
{
	srandom( time( NULL ) );

	uint8_t bla[ 16 * 16 ];
	for( int y = 0; y < 16; y++ ) {
		for( int x = 0; x < 16; x++ ) {
			bla[ y * 16 + x ] = ( uint8_t ) Math::rand( 0x0, 0xff);
			std::cout << " " << ( int ) bla[ y * 16 + x ];
		}
		std::cout << std::endl;
	}

	float xx, yy, xy;
	float harris = SIMD::get( SIMD_BASE )->harrisResponse1u8( xx, xy, yy, bla + 7 * 16 + 7, 16, 7, 7, 0.04f );
	std::cout << "SIMD_BASE: " << harris << " XX:" << xx << " XY:" << xy << " YY:" <<  yy << std::endl;

#if 1
	for( int y = 0; y < 15; y++ ) {
	//	std::cout << std::endl << "dx " << y;
	//	for( int x = 1; x < 15; x++ ) {
	//		std::cout << " " << ( int ) bla[ y * 16 + x - 1 ] - ( int ) bla[ y * 16 + x + 1 ];
	//	}
		std::cout << std::endl << "sob dx";
	for( int x = 1; x < 15; x++ ) {
			std::cout << " " <<  ( int ) bla[ y * 16 + x - 1 ] - ( int ) bla[ y * 16 + x + 1 ]
+ 2 * ( ( int ) bla[ y * 16 + 16 + x - 1 ] - ( int ) bla[ y * 16 + 16 + x + 1 ] )
+ ( int ) bla[ y * 16 + 32 + x - 1 ] - ( int ) bla[ y * 16 + 32 + x + 1 ];
		}
	}
	for( int y = 0; y < 15; y++ ) {
		//std::cout << "dy " << y;
		//for( int x = 0; x < 16; x++ ) {
		//	std::cout << " " << ( int ) bla[ y * 16 + x ] - ( int ) bla[ y * 16 + x + 32 ];
		//}
		std::cout << std::endl << "sob dy";
		for( int x = 1; x < 15; x++ ) {
			std::cout << " " <<  ( ( int ) bla[ y * 16 + x - 1 ] - ( int ) bla[ y * 16 + x + 32 - 1 ] )
+ 2 * ( ( int ) bla[ y * 16 + x  ] - ( int ) bla[ y * 16 + x + 32  ] )
 + ( ( int ) bla[ y * 16 + x + 1 ] - ( int ) bla[ y * 16 + x + 32 + 1 ] );
		}
	}
	std::cout << std::endl;
#endif

	harris = SIMD::get( SIMD_SSE2 )->harrisResponseCircular1u8( xx, xy, yy, bla + 7 * 16 + 7, 16, 0.04f );
	std::cout << "SIMD_SSE2: " << harris << " XX:" << xx << " XY:" << xy << " YY:" <<  yy << std::endl;
}
