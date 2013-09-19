#include <cvt/gfx/Image.h>
#include <cvt/gfx/IMapScoped.h>

using namespace cvt;

int main( int argc, char** argv )
{
    Image img( argv[ 1 ] );
    Image otherf( img.width(), img.height(), IFormat::floatEquivalent( img.format() ) );
    Image other16( img.width(), img.height(), IFormat::uint16Equivalent( img.format() ) );
    img.convert( otherf );
    otherf.save( "imgfloat.png" );
    otherf.convert( other16 );
    other16.save( "imgu16.png" );

    std::cout << otherf << "\n" << other16 << std::endl;

/*    Image bla( 2, 2, IFormat::GRAY_UINT16 );
    {
        IMapScoped<uint16_t> map( bla );
        map( 0, 0 ) = 0;
        map( 0, 1 ) = 0xff;
        map( 1, 0 ) = 0xff00;
        map( 1, 1 ) = 0xffff;
    }

    bla.save( "tmp16.png" );

    bla.load( "tmp16.png" );
    {
        IMapScoped<uint16_t> map( bla );
        std::cout << map( 0, 0 ) << std::endl;
        std::cout << map( 0, 1 ) << std::endl;
        std::cout << map( 1, 0 ) << std::endl;
        std::cout << map( 1, 1 ) << std::endl;
    }*/


    return 0;
}
