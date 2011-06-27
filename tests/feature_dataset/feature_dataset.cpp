#include <iostream>


#include <cvt/gui/Application.h>
#include <cvt/gui/Window.h>
#include <cvt/gui/ImageView.h>
#include <cvt/gui/Moveable.h>

#include <cvt/gfx/Image.h>
#include <cvt/io/Resources.h>

#include <cvt/gfx/Color.h>
#include <cvt/util/Exception.h>

#include <string>

using namespace cvt;

int main()
{
	cvt::Resources resources;
    
    String datasetFolder = resources.find( "features_dataset" );
    std::cout << "Dataset Folder: " << datasetFolder << std::endl;
	//String graffiti = resources.find( "features_dataset/graf/img1.ppm" );
	//String graffiti = resources.find( "ppmtest.ppm" );
	String graffiti = resources.find( "ppmtestc.ppm" );

	cvt::Image graf;	
	try {
		graf.load( graffiti.c_str() );
	} catch( cvt::Exception e ) {
		std::cerr << e.what() << std::endl;
		return 1;
	}

	Window win( "Feature Detector and Descriptor Test" );
	win.setSize( 800, 600 );
	win.setVisible( true );
	win.setMinimumSize( 320, 240 );

	ImageView imView;
	imView.setImage( graf );
	Moveable mov( &imView );
	mov.setSize( graf.width(), graf.height() );
	win.addWidget( &mov );

	Application::run();
	
	return 0;
}
