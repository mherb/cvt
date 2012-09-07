#include <cvt/gfx/Image.h>
#include <cvt/cl/OpenCL.h>
#include <cvt/cl/CLPlatform.h>
#include <cvt/cl/CLKernel.h>
#include <cvt/cl/CLContext.h>
#include <cvt/cl/CLBuffer.h>
#include <cvt/util/String.h>
#include <cvt/util/Time.h>
#include <cvt/util/Data.h>
#include <cvt/util/DataIterator.h>
#include <cvt/io/FileSystem.h>

#include <cvt/vision/TSDFVolume.h>
#include <cvt/vision/CameraCalibration.h>


#define VOL_WIDTH  400
#define VOL_HEIGHT 400
#define VOL_DEPTH  400

using namespace cvt;

int main( int argc, char** argv )
{
    if( argc < 2 ){
        std::cout << "Usage: " << argv[ 0 ] << " datafile" << std::endl;
        return 0;
    }

    std::vector<CLPlatform> platforms;
    CLPlatform::get( platforms );
    CL::setDefaultDevice( platforms[ 0 ].defaultDevice() );

    try {

        Matrix4f gridToWorld( 0.2f / ( float )( VOL_WIDTH ), 0.0f, 0.0f,  -0.1f,
                              0.0f, 0.2f / ( float )( VOL_HEIGHT ), 0.0f, -0.00f,
                              0.0f, 0.0f, 0.2f / ( float ) ( VOL_DEPTH ), -0.1f,
                              0.0f, 0.0f, 0.0f, 1.0f );


		TSDFVolume tsdf( gridToWorld, VOL_WIDTH, VOL_HEIGHT, VOL_DEPTH, 0.01f );
		tsdf.clear();

        Image depthmap, tmp;

        Time t;
        /* add the depth maps */

		Data datafile;
        FileSystem::load( datafile, argv[ 1 ] );
        DataIterator iter( datafile );

        String name;
		int i = 10;
        while( iter.hasNext() && i-- ){
			std::vector<String> tokens;
			iter.tokenizeNextLine( tokens, " " );


			String dmapfile;
			dmapfile = tokens[ 0 ].substring( 0, tokens[ 0 ].length() - 4 ) + "_depthmap.cvtraw";
			std::cout << dmapfile << std::endl;
			String calibfile = tokens[ 0 ] + ".xml";
			std::cout << calibfile << std::endl;

			tmp.load( dmapfile );
            tmp.convert( depthmap, IFormat::RGBA_FLOAT, IALLOCATOR_CL );

			CameraCalibration calib;
			calib.load( calibfile );

			tsdf.addDepthMap( calib.intrinsics(), calib.extrinsics(), depthmap,  ( float ) 8.0f  );
        }
        std::cout << t.elapsedMilliSeconds() << " ms" << std::endl;

		tsdf.saveRaw( "tsdf2.raw" );


    } catch( CLException& e ) {
        std::cout << e.what() << std::endl;
    }

    return 0;
}
