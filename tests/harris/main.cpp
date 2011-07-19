#include <cvt/util/SIMD.h>
#include <cvt/math/Math.h>

#include <stdio.h>

#include <cvt/util/SIMDDebug.h>

using namespace cvt;

int main()
{
	srandom( time( NULL ) );

	uint8_t bla[ 9 * 9 ];
	for( int y = 0; y < 9; y++ ) {
		for( int x = 0; x < 9; x++ ) {
			bla[ y * 9 + x ] = ( uint8_t ) Math::rand( 0x0, 0xff);
			std::cout << " " << ( int ) bla[ y * 9 + x ];
		}
		std::cout << std::endl;
	}

	float harris = SIMD::get( SIMD_BASE )->harrisResponse1u8( bla + 4 * 9 + 4, 9, 4, 4, 0.04f );
	std::cout << "SIMD_BASE: " << harris << std::endl;
	harris = SIMD::get( SIMD_SSE2 )->harrisResponse1u8( bla + 4 * 9 + 4, 9, 4, 4, 0.04f );
	std::cout << "SIMD_SSE2: " << harris << std::endl;
}
