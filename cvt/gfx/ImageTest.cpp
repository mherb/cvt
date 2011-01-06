#include <cvt/gfx/Image.h>
#include <cvt/util/CVTTest.h>
#include <cvt/io/ImageIO.h>
#include <cvt/io/Resources.h>
#include <cvt/util/SIMD.h>

namespace cvt {

	static void _image_conversion_test()
	{



	}

	BEGIN_CVTTEST( image )
		Color color( 255, 0, 0, 255 );
		Image y;
		uint32_t val;
		uint8_t* valPtr;
		bool b;
		float* base;
		size_t stride;

		Image x( 1, 1, IFormat::RGBA_UINT8 );
		x.fill( color );

		std::cerr << "RGBA UBYTE TO:" << std::endl;

		valPtr = x.map( &stride );
		val = *( ( uint32_t* ) valPtr );
		CVTTEST_PRINT("RGBA UBYTE", val == 0xFF0000FF );
		x.unmap( valPtr );

		x.convert( y, IFormat::BGRA_UINT8 );
		valPtr = y.map( &stride );
		val = *( ( uint32_t* ) valPtr );
		CVTTEST_PRINT("BGRA UBYTE", val == 0xFFFF0000 );
		y.unmap( valPtr );

		x.convert( y, IFormat::RGBA_FLOAT );
		base = ( float* ) y.map( &stride );
		b  = *( base + 0 ) == 1.0f;
		b &= *( base + 1 ) == 0.0f;
		b &= *( base + 2 ) == 0.0f;
		b &= *( base + 3 ) == 1.0f;
		CVTTEST_PRINT("RGBA FLOAT", b );
		y.unmap( ( uint8_t* ) base );

		x.convert( y, IFormat::BGRA_FLOAT );
		base = ( float* ) y.map( &stride );
		b  = *( base + 0 ) == 0.0f;
		b &= *( base + 1 ) == 0.0f;
		b &= *( base + 2 ) == 1.0f;
		b &= *( base + 3 ) == 1.0f;
		CVTTEST_PRINT("BGRA FLOAT", b );
		y.unmap( ( uint8_t* ) base );

		x.reallocate( 1, 1, IFormat::BGRA_FLOAT );
		x.fill( color );
		std::cerr << "RGBA FLOAT TO:" << std::endl;

		y.reallocate( 1, 1, IFormat::RGBA_UINT8 );
		x.convert( y );
		
		valPtr = y.map( &stride );
		val = *( ( uint32_t* ) valPtr );		
		CVTTEST_PRINT("RGBA UBYTE", val == 0xFF0000FF );
		y.unmap( valPtr );

		y.reallocate( 1, 1, IFormat::BGRA_UINT8 );
		x.convert( y );
		valPtr = y.map( &stride );
		val = *( ( uint32_t* ) valPtr );		
		CVTTEST_PRINT("BGRA UBYTE", val == 0xFFFF0000 );
		y.unmap( valPtr );

		y.reallocate( 1, 1, IFormat::RGBA_FLOAT );
		x.convert( y );
		base = ( float* ) y.map( &stride );
		b  = *( base + 0 ) == 1.0f;
		b &= *( base + 1 ) == 0.0f;
		b &= *( base + 2 ) == 0.0f;
		b &= *( base + 3 ) == 1.0f;
		CVTTEST_PRINT("RGBA FLOAT", b );
		y.unmap( ( uint8_t* ) base );

		y.reallocate( 1, 1, IFormat::BGRA_FLOAT );
		x.convert( y );
		base = ( float* ) y.map( &stride );
		b  = *( base + 0 ) == 0.0f;
		b &= *( base + 1 ) == 0.0f;
		b &= *( base + 2 ) == 1.0f;
		b &= *( base + 3 ) == 1.0f;
		CVTTEST_PRINT("BGRA FLOAT", b );
		y.unmap( ( uint8_t* ) base );


		CVTTEST_LOG("Image Convert Speed BASE");
		SIMD::force( SIMD_BASE );
		_image_conversion_test();
		CVTTEST_LOG("Image Convert Speed SSE");
		SIMD::force( SIMD_SSE );
		_image_conversion_test();

		SIMD::force( SIMD_BEST );

		return true;
	END_CVTTEST
}
