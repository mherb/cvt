#include <fstream>

#include <cvt/gui/Window.h>
#include <cvt/gui/WidgetLayout.h>
#include <cvt/gui/Application.h>
#include <cvt/vision/Vision.h>
#include <cvt/vision/CameraCalibration.h>

#include "GLSceneView.h"

using namespace cvt;

void scenepointsToPly( const String& file, const ScenePoints& pts )
{

	if( pts.vertexSize() != pts.colorSize() )
		return;

	std::ofstream outfile;
	outfile.open ( file.c_str() );

	outfile << "ply\n";
	outfile << "format ascii 1.0\n";
	outfile << "element vertex " << pts.vertexSize() << "\n";
	outfile << "property float x\n";
	outfile << "property float y\n";
	outfile << "property float z\n";
	outfile << "property uchar red\n";
	outfile << "property uchar green\n";
	outfile << "property uchar blue\n";
	outfile << "end_header\n";

	size_t n = pts.vertexSize();
	const Vector3f* vptr = pts.vertices();
	const Vector4f* cptr = pts.colors();

#define CONV( n ) ( ( int ) ( n * 255.0f ) )

	for( size_t i = 0; i < n; i++ ) {
		outfile << vptr->x << " " << vptr->y << " " << vptr->z << " "
				<< CONV( cptr->x ) << " " << CONV (cptr->y ) << " " << CONV( cptr->z ) << "\n";
		vptr++;
		cptr++;
	}

	outfile.close();
}

int main( int argc, char** argv )
{
	if( argc < 3 ){
		std::cout << "Usage: " << argv[ 0 ] << " image depthmap scale" << std::endl;
		return 0;
	}

	try {
		Image image( argv[ 1 ] ), depthmap( argv[ 2 ] ), mask( argv[ 4 ]);
		float scale = String( argv[ 3 ] ).toFloat();

		CameraCalibration camcalib;
		camcalib.setIntrinsics( Matrix3f( 800.0f, 0.0f, 0.5f * ( float ) image.width(),
										 0.0f,   800.0f, 0.5f * ( float ) image.height(),
										 0.0,	  0.0f,  1.0f ) );
		ScenePoints pts( "Points" );

		Image c, dmap, msk;
		image.convert( c, IFormat::RGBA_FLOAT );
		depthmap.convert( dmap, IFormat::GRAY_FLOAT );
		mask.convert( msk, IFormat::GRAY_FLOAT );
		dmap.mul( msk );
		Vision::unprojectToScenePoints( pts, c, dmap, scale);

//		pts.translate( -pts.centroid() );
		scenepointsToPly("test.ply", pts );

		Window w( "meshView" );
		w.setSize( 1280, 720 );
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
