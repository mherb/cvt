#include <iostream>

#include <cvt/gfx/Image.h>
#include <cvt/gfx/ifilter/ROFDenoise.h>
#include <cvt/io/ImageIO.h>
#include <cvt/gfx/Color.h>

#include <cvt/gui/Display.h>

#include <string>

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);
    
    std::string dataFolder(DATA_FOLDER);
    std::string inputFile(dataFolder + "/lena.png");
        
    // RGBA UBYTE IMAGE
    cvt::Image img(640, 480);    
    cvt::ImageIO::loadPNG(img, inputFile);
        
    cvt::Display display;
    display.addImage(img);
            
    display.show();
    
    return app.exec();    
}