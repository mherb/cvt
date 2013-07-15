#include <cvt/gui/Window.h>
#include <cvt/gui/WidgetLayout.h>
#include <cvt/gui/Application.h>
#include <cvt/vision/Vision.h>
#include <cvt/vision/CameraCalibration.h>
#include <cvt/io/RGBDParser.h>

#include "GLSceneView.h"

using namespace cvt;

int main( int argc, char** argv )
{
    if( argc < 2 ){
        std::cout << "Usage: " << argv[ 0 ] << " <data_set_folder>" << std::endl;
        return 0;
    }

    try {
        String folder( argv[ 1 ] );

        RGBDParser rgbddata( folder );

        CameraCalibration camcalib;
        camcalib.setIntrinsics( Matrix3f( 517.3f, 0.0f, 318.6f,
                                         0.0f,   516.5f, 255.3f,
                                         0.0,	  0.0f,  1.0f ) );

        ScenePoints pts( "Points" );
        for( int i = 0; i < 30; i++) {
            rgbddata.nextFrame();
            camcalib.setExtrinsics( rgbddata.data().pose<float>().inverse() );
//			std::cout << rgbddata.data().rgb << std::endl;
//			std::cout << rgbddata.data().depth << std::endl;
            Image depthmap, image;
            rgbddata.data().rgb.convert( image, IFormat::RGBA_FLOAT );
            rgbddata.data().depth.convert( depthmap, IFormat::GRAY_FLOAT );
//			depthmap.save( "map.png" );
            ScenePoints ptsnew( "BLA" );
            Vision::unprojectToScenePoints( ptsnew, image, depthmap, camcalib, ( float ) ( 0xffff ) / 5000.0f );
            pts.add( ptsnew );
        }
//		pts.translate( -pts.centroid() );
//		pts.scale( 1e6f );

        std::cout << pts.boundingBox() << std::endl;


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

    } catch ( cvt::Exception& e ){
        std::cout << e.what() << std::endl;
    }


    return 0;

}
