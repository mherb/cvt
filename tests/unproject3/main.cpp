#include <cvt/gui/Window.h>
#include <cvt/gui/WidgetLayout.h>
#include <cvt/gui/Application.h>
#include <cvt/vision/Vision.h>
#include <cvt/vision/CameraCalibration.h>
#include <cvt/vision/StereoCameraCalibration.h>
#include <cvt/io/RGBDParser.h>

#include "GLSceneView.h"

using namespace cvt;

int main( int argc, char** argv )
{
    if( argc != 5 ){
        std::cout << "Usage: " << argv[ 0 ] << "stereo-calib image disparity dscale" << std::endl;
        return 0;
    }

    try {

        StereoCameraCalibration calib;
		calib.load( argv[ 1 ] );
		float dispscale = String( argv[ 4 ] ).toFloat();

        ScenePoints pts( "Points" );
		Image tmp, depthmap, disparity, image;
		tmp.load( argv[ 2 ] );
        tmp.convert( image, IFormat::RGBA_FLOAT );
		tmp.load( argv[ 3 ] );
        tmp.convert( disparity, IFormat::GRAY_FLOAT );
		Vision::disparityToDepthmap( depthmap, disparity, dispscale, calib.focalLength(), calib.baseLine() );

//		disparity.save("disparity.cvtraw");
//		depthmap.mul( 1.0f / 40.0f );
//		depthmap.save("depthmap.cvtraw");
		ScenePoints ptsnew( "BLA" );
		Vision::unprojectToScenePoints( ptsnew, image, depthmap, calib.firstCamera(), 1.0f );
		pts.add( ptsnew );
//		pts.scale( 0.2 );
//		pts.translate( -pts.centroid() );

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
