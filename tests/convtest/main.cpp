#include <cvt/gfx/Image.h>
#include <cvt/gfx/IMapScoped.h>
#include <cvt/cl/OpenCL.h>
#include <cvt/cl/CLPlatform.h>
#include <cvt/cl/CLKernel.h>
#include <cvt/cl/CLContext.h>
#include <cvt/util/Time.h>

#include <cvt/cl/kernel/fill.h>

using namespace cvt;

int main( int argc, char** argv )
{
	std::vector<CLPlatform> platforms;
	std::vector<CLDevice> devs;

	CLPlatform::get( platforms );
	platforms[ 0 ].devices( devs, CL_DEVICE_TYPE_GPU );
	CL::setDefaultDevice( devs[ 0 ] );

    Image img( argv[ 1 ] );
    Image otherf( img.width(), img.height(), IFormat::floatEquivalent( img.format() ) );
    Image other16( img.width(), img.height(), IFormat::uint16Equivalent( img.format() ) );
    img.convert( otherf );
    otherf.save( "imgfloat.png" );
    otherf.convert( other16 );
    other16.save( "imgu16.png" );

    std::cout << otherf << "\n" << other16 << std::endl;

    Image bla( 2, 2, IFormat::GRAY_UINT16 );
    {
        IMapScoped<uint16_t> map( bla );
        map( 0, 0 ) = 0;
        map( 0, 1 ) = 0xff;
        map( 1, 0 ) = 0xff00;
        map( 1, 1 ) = 0xffff;
    }

    bla.save( "tmp16.png" );

	CLKernel clfill( _fill_source, "fill" );
    Image dmap( 2, 2, IFormat::GRAY_UINT16, IALLOCATOR_CL );
    cl_float4 val = { 0.5f, 0.0f, 0.0f, 1.0f };
    clfill.setArg( 0, dmap );
    clfill.setArg( 1, val );
    clfill.runWait( CLNDRange( Math::pad( dmap.width(), 16 ), Math::pad( dmap.height(), 16 ) ), CLNDRange( 16, 16 ) );
    dmap.save("tmp16.png");


    bla.load( "tmp16.png" );
    {
        IMapScoped<uint16_t> map( bla );
        std::cout << map( 0, 0 ) << std::endl;
        std::cout << map( 0, 1 ) << std::endl;
        std::cout << map( 1, 0 ) << std::endl;
        std::cout << map( 1, 1 ) << std::endl;
    }

    return 0;
}
