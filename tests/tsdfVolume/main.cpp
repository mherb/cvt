#include <cvt/gfx/Image.h>
#include <cvt/cl/OpenCL.h>
#include <cvt/cl/CLPlatform.h>
#include <cvt/cl/CLKernel.h>
#include <cvt/cl/CLContext.h>
#include <cvt/cl/CLBuffer.h>
#include <cvt/util/String.h>
#include <cvt/util/Time.h>

#include <cvt/cl/kernel/TSDFVolume/TSDFVolume.h>

#include <cvt/io/RGBDParser.h>

#define VOL_WIDTH  256
#define VOL_HEIGHT 256
#define VOL_DEPTH  256

using namespace cvt;

int main( int argc, char** argv )
{
    if( argc < 2 ){
        std::cout << "Usage: " << argv[ 0 ] << " <data_set_folder>" << std::endl;
        return 0;
    }



    std::vector<CLPlatform> platforms;
    CLPlatform::get( platforms );
    CL::setDefaultDevice( platforms[ 0 ].defaultDevice() );

    try {

        String folder( argv[ 1 ] );

        RGBDParser rgbddata( folder );

        Matrix3f intrinsics( 517.3f, 0.0f, 318.6f,
                               0.0f,   516.5f, 255.3f,
                                0.0,	  0.0f,  1.0f );

        Matrix4f gridToWorld( 2.0f / ( float )( VOL_WIDTH ), 0.0f, 0.0f, -0.2f,
                              0.0f, 2.0f / ( float )( VOL_HEIGHT ), 0.0f, 0.0f,
                              0.0f, 0.0f, 2.0f / ( float ) ( VOL_DEPTH ), 0.4f,
                              0.0f, 0.0f, 0.0f, 1.0f );

        Matrix4f gridToCam;


        CLKernel kernclr( _TSDFVolume_source, "TSDFVolume_clear" );
        CLKernel kernadd( _TSDFVolume_source, "TSDFVolume_add" );
        CLKernel kernslice( _TSDFVolume_source, "TSDFVolume_slice" );

        CLBuffer cv( sizeof( cl_float2 ) * VOL_WIDTH * VOL_HEIGHT * VOL_DEPTH  );


        /* clear the volume */
        kernclr.setArg( 0, cv );
        kernclr.setArg( 1, VOL_WIDTH );
        kernclr.setArg( 2, VOL_HEIGHT );
        kernclr.setArg( 3, VOL_DEPTH );
        kernclr.run( CLNDRange( Math::pad16( VOL_WIDTH ), Math::pad16( VOL_HEIGHT ), VOL_DEPTH ), CLNDRange( 16, 16, 1 ) );

        Image depthmap;

        CLBuffer clintrin( ( void*)intrinsics.toMatrix4().ptr(), sizeof( float ) * 12 );

        Time t;
        /* add the depth maps */
        for( int i = 0; i < 700; i++) {
            rgbddata.loadNext();
            rgbddata.data().depth.convert( depthmap, IFormat::GRAY_UINT16, IALLOCATOR_CL );
//			camcalib.setExtrinsics( rgbddata.data().pose );
//			rgbddata.data().rgb.convert( image, IFormat::RGBA_FLOAT );

            Matrix4f pose = rgbddata.data().pose<float>();
            gridToCam = pose.inverse() * gridToWorld;
            CLBuffer clglobal( ( void*)gridToCam.ptr(), sizeof( float ) * 12 );

            kernadd.setArg( 0, cv );
            kernadd.setArg( 1, VOL_WIDTH );
            kernadd.setArg( 2, VOL_HEIGHT );
            kernadd.setArg( 3, VOL_DEPTH );
            kernadd.setArg( 4, depthmap );
            kernadd.setArg( 5, ( float ) ( 0xffff ) / 5000.0f );
            kernadd.setArg( 6, clintrin );
            kernadd.setArg( 7, clglobal );
            kernadd.setArg( 8, 5.0f );
            kernadd.run( CLNDRange( Math::pad16( VOL_WIDTH ), Math::pad16( VOL_HEIGHT ), VOL_DEPTH ), CLNDRange( 16, 16, 1 ) );

        }
        std::cout << t.elapsedMilliSeconds() << " ms" << std::endl;

        {
            FILE* f;
            f = fopen("data.raw","wb");
            float* ptr = ( float* ) cv.map();
            float* origptr = ptr;
            size_t n = VOL_WIDTH * VOL_HEIGHT * VOL_DEPTH;
            while( n-- ) {
                fwrite( ptr, sizeof( float ), 1, f );
                ptr += 2;
            }
            fclose( f );
            cv.unmap( origptr );
        }

        /*depthmap.reallocate( VOL_WIDTH, VOL_HEIGHT, IFormat::GRAY_FLOAT, IALLOCATOR_CL );

        for( int i = 0; i < 512; i++ ) {
            kernslice.setArg( 0, depthmap );
            kernslice.setArg( 1, i );
            kernslice.setArg( 2, cv );
            kernslice.setArg( 3, VOL_WIDTH );
            kernslice.setArg( 4, VOL_HEIGHT );
            kernslice.setArg( 5, VOL_DEPTH );
            kernslice.run( CLNDRange( Math::pad16( VOL_WIDTH ), Math::pad16( VOL_HEIGHT ) ), CLNDRange( 16, 16 ) );
            String name;
            name.sprintf("slice%03d.png", i );
            depthmap.save( name );
        }*/

    } catch( CLException& e ) {
        std::cout << e.what() << std::endl;
    }

    return 0;
}
