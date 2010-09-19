#include <cvt/gfx/Image.h>
#include <cvt/math/Math.h>
#include <cvt/vision/Flow.h>
#include <cvt/io/FloFile.h>

using namespace cvt;

int main()
{
#define SCALE 50.0f
	Image i( 200, 200, IOrder::GRAYALPHA, IType::FLOAT );
	uint8_t* base = i.map();
	ssize_t istride = i.stride();
	for( size_t y = 0; y < 200; y++ ) {
		for( size_t x = 0; x < 200; x++ ) {
			float xf = ( ( float ) x ) - 100.0f;
			float yf = ( ( float ) y ) - 100.0f;
			float d = Math::sqrt( xf * xf + yf * yf );
			float c[ 2 ];
			if( d > 100.0f ) {
				c[ 0 ] = 0.0f;
			   	c[ 1 ] = 0.0f;
			} else {
				c[ 0 ] = xf / SCALE;
				c[ 1 ] = yf / SCALE;
			}
			*( ( ( float* ) ( base + y * istride ) ) + 2 * x ) = c[ 0 ];
			*( ( ( float* ) ( base + y * istride ) ) + 2 * x + 1 ) = c[ 1 ];
		}
	}
	i.unmap();

	FloFile::FloWriteFile( i, "colorcode.flo" );
}
