#include <cvt/math/FFT.h>
#include <cvt/util/SIMD.h>
#include <cvt/gfx/Image.h>

#include <cvt/gfx/IMapScoped.h>

using namespace cvt;

int main( int argc, char** argv )
{
	size_t i;
    float data[ 8 * 2 ] = { 1.0f, 0.0f,
			2.0f, 0.0f,
			0.0f, 0.0f,
			3.0f, 0.0f,
			8.0f, 0.0f,
			4.0f, 0.0f,
			2.0f, 0.0f,
			1.0f, 0.0f };
    float fftdata[ 8 * 2 ];

	SIMD* simd = SIMD::instance();
	simd->Memcpy( ( uint8_t* ) fftdata, ( uint8_t* ) data, sizeof( float ) * 16 );

	FFT::fftStridedRadix2( ( Complexf* ) fftdata, 8, 1, false );
    FFT::fftStridedRadix2( ( Complexf* ) fftdata, 8, 1, true );
	for( i = 0; i < 8; i++ ) {
		if( Math::abs( fftdata[ i * 2 ] - data[ i * 2 ] ) > 1e-5f ) {
			std::cout << "FFT forward/backward diff: %g > 1e-5f\n" << Math::abs( fftdata[ i * 2 ] - data[ i * 2 ] ) << std::endl;
		}
	}

	Image in( argv[ 1 ] );
	Image fin( in.width(), in.height(), IFormat::GRAY_FLOAT );
	in.convert( fin );
	{
		IMapScoped<float> map( fin );
		size_t xend = map.width();
		size_t yend = map.height();
		size_t stride = map.stride() / ( 2 * sizeof( float ) );
		for( size_t y = 0; y < yend; y++ ){
			FFT::fftRadix2( ( Complexf* ) map.ptr(), map.width() / 2, false );
			map++;
		}
		map.reset();
	//	for( size_t x = 0; x < xend; x++ ){
	//		FFT::fftStridedRadix2( ( Complexf* ) ( map.ptr() + x ), map.height() / 2, stride, false );
	//	}
	}
	{
		IMapScoped<float> map( fin );
		size_t xend = map.width();
		size_t yend = map.height();
		size_t stride = map.stride() / ( 2 * sizeof( float ) );
		for( size_t y = 0; y < yend; y++ ){
			FFT::fftRadix2( ( Complexf* ) map.ptr(), map.width() / 2, true );
			map++;
		}
		map.reset();
	//	for( size_t x = 0; x < xend; x++ ){
	//		FFT::fftStridedRadix2( ( Complexf* ) ( map.ptr() + x ), map.height() / 2, stride, true );
	//	}
	}
	fin.save("ffttest.png");

}
