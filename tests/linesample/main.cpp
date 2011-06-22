#include <cvt/gfx/Image.h>
#include <cvt/gfx/GFX.h>
#include <cvt/gfx/GFXEngineImage.h>

using namespace cvt;

static bool sampleNormal( uint8_t* ptr, uint8_t th, int _x, int _y, int x1, int y1, int x2, int y2, int n, size_t w, size_t h, size_t bpp, size_t stride, float& dist )
{
	int dy = - Math::abs( x2 - x1 );
	int dx =   Math::abs( y2 - y1 );
	ssize_t incx, incy;
	int sx, sy;
	int x, y;
	int err = dx + dy;
	int e2;

	if( y2 < y1 ) {
		incx = -bpp;
		sx   = -1;
	} else {
		incx = bpp;
		sx   = 1;
	}

	if( x2 < x1 ) {
		incy = stride;
		sy   = 1;
	} else {
		incy = -stride;
		sy   = -1;
	}
	x = y = 0;

	uint8_t* dst1 = ptr + _x * bpp + _y * stride;
	uint8_t* dst2 = dst1;

	while( n-- ) {
		if( ( ( size_t ) ( _x + x ) ) < w && ( ( size_t ) ( _y + y ) ) < h ) {
/*			if( *dst1 >= th ) {
				dist = Math::sqrt( ( float ) ( Math::sqr( x ) + Math::sqr( y ) ) );
				return true;
			}*/
			*dst1 = th + 0x20;
		}
		if( ( ( size_t ) ( _x - x ) ) < w && ( ( size_t ) ( _y - y ) ) < h ) {
/*			if( *dst2 >= th ) {
				dist = -Math::sqrt( ( float ) ( Math::sqr( x ) + Math::sqr( y ) ) );
				return true;
			} */
			*dst2 = th - 0x20;
		}
		e2 = 2 * err;
		if( e2 >= dy ) { err += dy; dst1 += incx; dst2 -= incx; x += sx; }
		if( e2 <= dx ) { err += dx; dst1 += incy; dst2 -= incy; y += sy; }
	}
	return false;
}

int main()
{
	srand( time( NULL ) );
	Image out( 640, 480, IFormat::GRAY_UINT8 );
	int x1, x2, y1, y2;

	x1 = Math::rand( 0, 640 );
	y1 = Math::rand( 0, 480 );
	x2 = Math::rand( 0, 640 );
	y2 = Math::rand( 0, 480 );



	{
		GFXEngineImage ge( out );
		GFX g( &ge );
		g.color().set( 0.0f, 0.0f, 0.0f, 1.0f );
		g.fillRect( 0, 0, 640, 480 );
		g.color().set( 1.0f, 1.0f, 1.0f, 1.0f );
		g.drawLine( x1, y1, x2, y2 );
		g.color().set( 1.0f, 1.0f, 1.0f, 1.0f );
		g.fillRect( x1 - 2, y1 - 2, 4, 4 );
		g.color().set( 0.4f, 0.4f, 0.4f, 1.0f );
		g.fillRect( x2 - 2, y2 - 2, 4, 4 );

	}

	{
		uint8_t* ptr;
		size_t stride, bpp;
		int n;

		n = 20;
		bpp = out.bpp();
		ptr = out.map<uint8_t>( &stride );
		float val;
		sampleNormal( ptr, 0xff >> 1, ((x1+x2)>>1),((y1+y2)>>1), x1, y1, x2, y2, n, out.width(), out.height(), bpp, stride, val );
		out.unmap( ptr );
	}

	out.save( "linesample.png" );
}
