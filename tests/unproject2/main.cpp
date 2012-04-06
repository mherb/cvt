#include <cvt/gui/Window.h>
#include <cvt/gui/WidgetLayout.h>
#include <cvt/gui/Application.h>
#include <cvt/vision/Vision.h>
#include <cvt/vision/CameraCalibration.h>

#include "GLSceneView.h"

using namespace cvt;

int main( int argc, char** argv )
{
	if( argc < 2 ){
		std::cout << "Usage: " << argv[ 0 ] << " image depthmap scale" << std::endl;
		return 0;
	}

	try {
		Image image( argv[ 1 ] ), depthmap( argv[ 2 ] );
		float scale = String( argv[ 3 ] ).toFloat();

		CameraCalibration camcalib;
		camcalib.setIntrinsics( Matrix3f( 800.0f, 0.0f, 0.5f * ( float ) image.width(),
										 0.0f,   800.0f, 0.5f * ( float ) image.height(),
										 0.0,	  0.0f,  1.0f ) );
		ScenePoints pts( "Points" );

		Image c, dmap;
		image.convert( c, IFormat::RGBA_FLOAT );
		depthmap.convert( dmap, IFormat::GRAY_FLOAT );
		Vision::unprojectToScenePoints( pts, c, dmap scale);

		pts.translate( -pts.centroid() );


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
