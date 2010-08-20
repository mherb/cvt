#include <QtGui>
#include <iostream>

#include <cvt/gfx/Image.h>
#include <cvt/gfx/ifilter/ROFDenoise.h>
#include <cvt/io/ImageIO.h>
#include <cvt/gfx/Color.h>

#include <cvt/gui/GLView.h>
#include <cvt/gui/GLObject.h>
#include <cvt/gui/GLImage.h>

#include <string>



int main(int argc, char* argv[])
{
	QApplication app(argc, argv);
	std::string dataFolder(DATA_FOLDER);
    std::string inputFile(dataFolder + "/lena.png");
	cvt::Image img;

    cvt::ImageIO::loadPNG(img, inputFile);

	cvt::GLView view;

	view.makeCurrent();

	cvt::GLImage obj( img );
	view.addGLObject( &obj );
	obj.setSize( 512, 512 );

	view.show();
	return app.exec();
}
