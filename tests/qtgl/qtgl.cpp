#include <QtGui>
#include <iostream>

#include <cvt/gfx/Image.h>
#include <cvt/gfx/ifilter/ROFDenoise.h>
#include <cvt/io/ImageIO.h>
#include <cvt/gfx/Color.h>
#include <cvt/util/Exception.h>

#include <cvt/gui/GLView.h>
#include <cvt/gui/GLObject.h>
#include <cvt/gui/GLImage.h>

#include <string>



void catch_global() {
	try {
		throw;
	}
	catch ( cvt::Exception e ) {
		std::cerr << "Exception: " << e.what() << std::endl;
	}
}



int main(int argc, char* argv[])
{
	std::set_terminate(catch_global);

	QApplication app(argc, argv);
	std::string dataFolder(DATA_FOLDER);
	std::string inputFile(dataFolder + "/lena.png");
	std::string inputFile2(dataFolder + "/lena_g.png");
	cvt::Image img;

	cvt::GLView view;

	view.makeCurrent();

	cvt::ImageIO::loadPNG(img, inputFile);
	cvt::GLImage obj( img );
	view.addGLObject( &obj );
	obj.setSize( 256, 256 );

	cvt::Image img2( img.width(), img.height(), cvt::CVT_RGBA, cvt::CVT_FLOAT );
	img.convert( img2 );
	cvt::GLImage obj3( img );
	view.addGLObject( &obj3 );
	obj3.setSize( 256, 256 );

	cvt::ImageIO::loadPNG(img, inputFile2 );
	cvt::GLImage obj2( img );
	view.addGLObject( &obj2 );
	obj2.setSize( 256, 256 );


	view.show();
	return app.exec();
}
