#include <cvt/gui/Window.h>
#include <cvt/gui/WidgetLayout.h>
#include <cvt/gui/Application.h>
#include <cvt/vision/Vision.h>
#include <cvt/vision/CameraCalibration.h>
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

#include "GLSceneView.h"

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

    try {

        Matrix4f gridToWorld( 0.2f / ( float )( VOL_WIDTH ), 0.0f, 0.0f,  -0.1f,
                              0.0f, 0.2f / ( float )( VOL_HEIGHT ), 0.0f, -0.00f,
                              0.0f, 0.0f, 0.2f / ( float ) ( VOL_DEPTH ), -0.1f,
                              0.0f, 0.0f, 0.0f, 1.0f );


        Image depthmap, tmp;

        Time t;
        /* add the depth maps */

		Data datafile;
        FileSystem::load( datafile, argv[ 1 ] );
        DataIterator iter( datafile );
        ScenePoints pts( "Points" );

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

			Image image, tmp2;
			tmp.load( tokens[ 0 ] );
			tmp.convert( image, IFormat::RGBA_FLOAT );
			tmp.convert( tmp2, IFormat::GRAY_FLOAT );

			Threshold thres;
			tmp.reallocate( tmp2.width(), tmp2.height(), IFormat::GRAY_FLOAT );
			thres.apply( tmp, tmp2, 0.001f );
			MorphErode erode;
			erode.apply( tmp2, tmp, 10 );
			MorphDilate dilate;
			dilate.apply( tmp, tmp2, 10 );
//			tmp.convert( tmp2, IFormat::RGBA_FLOAT );


			tmp2.load( dmapfile );
            tmp2.convert( depthmap, IFormat::GRAY_FLOAT );
			depthmap.mul( tmp );

			CameraCalibration calib;
			calib.load( calibfile );
			calib.setExtrinsics( calib.extrinsics() );

            ScenePoints ptsnew( "BLA" );
            Vision::unprojectToScenePoints( ptsnew, image, depthmap, calib, 8.0f  );
            pts.add( ptsnew );
        }
        std::cout << t.elapsedMilliSeconds() << " ms" << std::endl;

        Window w( "meshView" );
        w.setSize( 640, 480 );
        GLSceneView view( pts );

        WidgetLayout wl;
        wl.setAnchoredTopBottom( 0, 0 );
        wl.setAnchoredLeftRight( 0, 0 );
        w.addWidget( &view, wl );
        w.setVisible( true );

        w.update();

        Application::run();


    } catch( CLException& e ) {
        std::cout << e.what() << std::endl;
    }

    return 0;
}
