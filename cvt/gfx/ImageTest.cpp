#include <cvt/gfx/Image.h>
#include <cvt/util/CVTTest.h>
#include <cvt/io/ImageIO.h>
#include <cvt/io/Resources.h>
#include <cvt/util/SIMD.h>
#include <cvt/util/Time.h>

namespace cvt {

#define CONVTEST( x ) do { \
			mps = 0; \
			Image out( img.width(), img.height(), x ); \
			for( int i = 0; i < 100; i++ ) { \
				t.reset(); \
				img.convert( out ); \
				s = t.elapsedMicroSeconds(); \
				mps += ( img.width() * img.height() ) / s; \
			} \
			mps /= 100.0; \
			std::cerr << "\t" << img.format() << " -> " << out.format() << " conversion: " << mps << " MP/s" << std::endl; \
		} while( 0 )


	static void _image_conversion_speed( const Image& img )
	{
		Time t;
		float mps, s;

//		CONVTEST( IFormat::GRAY_UINT8 );
		CONVTEST( IFormat::BGRA_UINT8 );
		CONVTEST( IFormat::RGBA_UINT8 );
//		CONVTEST( IFormat::GRAY_FLOAT );
		CONVTEST( IFormat::BGRA_FLOAT );
		CONVTEST( IFormat::RGBA_FLOAT );
	}

	static void _image_conversionyuyv_speed( const Image& img )
	{
		Time t;
		float mps, s;

		CONVTEST( IFormat::BGRA_UINT8 );
		CONVTEST( IFormat::RGBA_UINT8 );
	}


	BEGIN_CVTTEST( Image )
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
		return true;
	END_CVTTEST

	BEGIN_CVTTEST( ImageSpeed )
		/* Image conversion */

		Image img;
		Resources res;
		std::string imgpath = res.find( "bbc-hd.png");
		std::string yuyvpath = res.find( "bbc-hd.yuyv");
		ImageIO::loadPNG( img, imgpath );
		Image imgt;
		Image imgyuyv( 1920, 1080, IFormat::UYVY_UINT8 );

		{
			size_t stride;
			uint8_t* base = imgyuyv.map<uint8_t>( &stride );
			uint8_t* dst = base;
			FILE* f = fopen( yuyvpath.c_str(), "r" );
			for( int i = 0; i < 1080; i++ ) {
				if( fread( dst, 1920, sizeof( uint8_t ) * 2, f ) != 1920 ) {
					fclose( f );
					return false;
				}
				dst += stride;
			}
			fclose( f );
			imgyuyv.unmap( base );
		}

		imgt.reallocate( img.width(), img.height(), IFormat::RGBA_UINT8 );
		imgyuyv.convert( imgt );
		ImageIO::savePNG( imgt, "vid.png" );

		CVTTEST_LOG("Image Convert Speed BASE");
		SIMD::force( SIMD_BASE );
		imgt.reallocate( img.width(), img.height(), IFormat::RGBA_UINT8 );
		img.convert( imgt );
		_image_conversion_speed( imgt );
		imgt.reallocate( img.width(), img.height(), IFormat::BGRA_UINT8 );
		img.convert( imgt );
		_image_conversion_speed( imgt );
		imgt.reallocate( img.width(), img.height(), IFormat::RGBA_FLOAT );
		img.convert( imgt );
		_image_conversion_speed( imgt );
		imgt.reallocate( img.width(), img.height(), IFormat::BGRA_FLOAT );
		img.convert( imgt );
		_image_conversion_speed( imgt );
		_image_conversionyuyv_speed( imgyuyv );

		CVTTEST_LOG("Image Convert Speed SSE");
		SIMD::force( SIMD_SSE );
		imgt.reallocate( img.width(), img.height(), IFormat::RGBA_UINT8 );
		img.convert( imgt );
		_image_conversion_speed( imgt );
		imgt.reallocate( img.width(), img.height(), IFormat::BGRA_UINT8 );
		img.convert( imgt );
		_image_conversion_speed( imgt );
		imgt.reallocate( img.width(), img.height(), IFormat::RGBA_FLOAT );
		img.convert( imgt );
		_image_conversion_speed( imgt );
		imgt.reallocate( img.width(), img.height(), IFormat::BGRA_FLOAT );
		img.convert( imgt );
		_image_conversion_speed( imgt );
		_image_conversionyuyv_speed( imgyuyv );

		CVTTEST_LOG("Image Convert Speed SSE41");
		SIMD::force( SIMD_SSE41 );
		imgt.reallocate( img.width(), img.height(), IFormat::RGBA_UINT8 );
		img.convert( imgt );
		_image_conversion_speed( imgt );
		imgt.reallocate( img.width(), img.height(), IFormat::BGRA_UINT8 );
		img.convert( imgt );
		_image_conversion_speed( imgt );
		imgt.reallocate( img.width(), img.height(), IFormat::RGBA_FLOAT );
		img.convert( imgt );
		_image_conversion_speed( imgt );
		imgt.reallocate( img.width(), img.height(), IFormat::BGRA_FLOAT );
		img.convert( imgt );
		_image_conversion_speed( imgt );
		_image_conversionyuyv_speed( imgyuyv );

		SIMD::force( SIMD_BEST );

		return true;
	END_CVTTEST
}
