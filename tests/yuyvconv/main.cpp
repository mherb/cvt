#include <cvt/gfx/Image.h>
#include <cvt/io/Resources.h>
#include <cvt/util/Util.h>

using namespace cvt;

int main()
{

	Resources res;
	String uyvypath = res.find( "bbc-hd.uyvy");
	String yuyvpath = res.find( "bbc-hd.yuyv");

	Image imguyvy( 1920, 1080, IFormat::UYVY_UINT8 );
	Image imgyuyv( 1920, 1080, IFormat::YUYV_UINT8 );

	Image out( 1920, 1080, IFormat::GRAYALPHA_UINT8 );

	{
		size_t stride;
		uint8_t* base = imguyvy.map<uint8_t>( &stride );
		uint8_t* dst = base;
		FILE* f = fopen( uyvypath.c_str(), "r" );
		for( int i = 0; i < 1080; i++ ) {
			if( fread( dst, sizeof( uint8_t ) * 2, 1920, f ) != 1920 ) {
				fclose( f );
				return false;
			}
			
			dst += stride;
		}
		fclose( f );
		imguyvy.unmap( base );
	}

	{
		size_t stride;
		uint8_t* base = imgyuyv.map<uint8_t>( &stride );
		uint8_t* dst = base;
		FILE* f = fopen( yuyvpath.c_str(), "r" );
		for( int i = 0; i < 1080; i++ ) {
			if( fread( dst, sizeof( uint8_t ) * 2, 1920, f ) != 1920 ) {
				fclose( f );
				return false;
			}
		/*	{
				uint16_t* ptr = ( uint16_t* ) dst;
				for( int x = 0; x < 1920; x++ ) {
					*ptr = Util::bswap16( *ptr );
					ptr++;
				}
			}*/
			dst += stride;
		}
		fclose( f );
		imgyuyv.unmap( base );
	}

	imguyvy.convert( out );
	out.save("uyvy2gray.png");
	imgyuyv.convert( out );
	out.save("yuyv2gray.png");
}
