#include <cvt/gfx/Image.h>
#include <cvt/gfx/IExpr.h>
#include <iostream>

using namespace cvt;


void print( const Image& img )
{
	IMapScoped<const float> map( img );

	for( size_t y = 0; y < img.height(); y++ ) {
		const float* ptr = map.ptr();
		for( size_t x = 0; x < img.width(); x++ ) {
			std::cout <<  ptr[ x ] << " ";
		}
		std::cout << std::endl;
		map++;
	}
}


int main()
{
	Image a( 5, 5, IFormat::GRAY_FLOAT );
	Image b( 5, 5, IFormat::GRAY_FLOAT );
	Image c( 5, 5, IFormat::GRAY_FLOAT );

	a.fill( Color( 2.0f ) );
	b.fill( Color( 1.0f ) );

	c = b * 0.25f + 0.5f;

	print( c );

	std::cout << a - b << std::endl;
	std::cout << -a * 10.0f + 2.0f + a -5.0f + b * 25 + -10.0f + a << std::endl;

	return 0;
}
