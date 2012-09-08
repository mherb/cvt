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

#include <cvt/gfx/ifilter/Threshold.h>
#include <cvt/gfx/ifilter/MorphErode.h>
#include <cvt/gfx/ifilter/MorphDilate.h>
#include <cvt/vision/TSDFVolume.h>
#include <cvt/vision/CameraCalibration.h>


#define VOL_WIDTH  256
#define VOL_HEIGHT 256
#define VOL_DEPTH  256

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
                              0.0f, 0.2f / ( float )( VOL_HEIGHT ), 0.0f, 0.0f,
                              0.0f, 0.0f, 0.2f / ( float ) ( VOL_DEPTH ), -0.1f,
                              0.0f, 0.0f, 0.0f, 1.0f );


		TSDFVolume tsdf( gridToWorld, VOL_WIDTH, VOL_HEIGHT, VOL_DEPTH, 0.01f );
		tsdf.clear( 3.0f );

        Image depthmap, tmp;

        Time t;
        /* add the depth maps */

		Data datafile;
        FileSystem::load( datafile, argv[ 1 ] );
        DataIterator iter( datafile );

        String name;
		int i = 0;
		int n = String( argv[ 2 ] ).toInteger();
        while( iter.hasNext() ){
			std::vector<String> tokens;
			iter.tokenizeNextLine( tokens, " " );


			i++;
			if( i > n )
				continue;

			String dmapfile;
			dmapfile = tokens[ 0 ].substring( 0, tokens[ 0 ].length() - 4 ) + "_depthmap.cvtraw";
			std::cout << dmapfile << std::endl;
			String calibfile = tokens[ 0 ] + ".xml";
			std::cout << calibfile << std::endl;

			Image gray;
			tmp.load( tokens[ 0 ] );
			tmp.convert( gray, IFormat::GRAY_FLOAT );
			Threshold thres;
			tmp.reallocate( gray.width(), gray.height(), IFormat::GRAY_FLOAT );
			thres.apply( tmp, gray, 0.01f );
			MorphErode erode;
			erode.apply( gray, tmp, 5 );
			MorphDilate dilate;
			dilate.apply( tmp, gray, 5 );
			tmp.convert( gray, IFormat::RGBA_FLOAT );


			tmp.load( dmapfile );
			tmp.mul( gray );
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
