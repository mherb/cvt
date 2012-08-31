#include <cvt/gui/Window.h>
#include <cvt/gui/WidgetLayout.h>
#include <cvt/gui/Application.h>
#include <cvt/vision/Vision.h>
#include <cvt/vision/CameraCalibration.h>

#include "GLSceneView.h"

using namespace cvt;

int main( int argc, char** argv )
{
    if( argc != 5 ){
        std::cout << "Usage: " << argv[ 0 ] << " calibration.xml image depthmap scale" << std::endl;
        return 0;
    }

    try {
        CameraCalibration camcalib;
		camcalib.load( argv[ 1 ] );
        Matrix4f id;
		id.setIdentity();
        camcalib.setExtrinsics( id );

        ScenePoints pts( "Points" );

        Image depthmap, image, tmp;
		float scale;
		String str( argv[ 4 ] );
		scale = str.toFloat();

		tmp.load( argv[ 2 ] );
		tmp.convert( image, IFormat::RGBA_FLOAT );
		tmp.load( argv[ 3 ] );
        tmp.convert( depthmap, IFormat::GRAY_FLOAT );

        Vision::unprojectToScenePoints( pts, image, depthmap, camcalib, scale );

        std::cout << pts.boundingBox() << std::endl;
		pts.translate( -pts.centroid() );
		pts.scale( 2.0f );


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
