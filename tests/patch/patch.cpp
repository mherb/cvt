#include <iostream>

#include <cvt/gfx/Image.h>
#include <cvt/util/Exception.h>
#include <cvt/geom/Rect.h>
#include <cvt/io/ImageIO.h>

#include <cvt/vision/Patch.h>

using namespace cvt;

void generateImage( Image & img )
{
    img.reallocate( 10, 10, IFormat::GRAY_UINT8 );
    
    size_t stride;
    uint8_t * ptr = img.map( &stride );
    
    uint8_t c = 0;
    for ( size_t i = 0; i < img.height(); i++ ) {
        for ( size_t x = 0; x < img.width(); x++ ) {
            ptr[ i * stride + x ] = c++;
        }
    }
    img.unmap( ptr );
}

int main()
{
	std::string dataFolder( DATA_FOLDER );
	std::string inputFile( dataFolder + "/lena_g.png" );
	
	try {
		// RGBA UBYTE IMAGE
		cvt::Image imgGray;		
		cvt::ImageIO::loadPNG( imgGray, inputFile );
		
		cvt::Image imgF( imgGray.width(), imgGray.height(), cvt::IFormat::floatEquivalent( imgGray.format() ) );
		imgGray.convert( imgF );
		
		cvt::Recti rect(100, 100, 100, 100);
		cvt::Image patchA(imgF, &rect, false);
		
		//rect.x = 100;
		cvt::Image patchB(imgF, &rect, false);		
		
		std::cout << "SSD: " << patchA.ssd( patchB ) << std::endl;
		std::cout << "SAD: " << patchA.sad( patchB ) << std::endl;
        
        Image simpleImage;
        generateImage( simpleImage );
        Recti roi( 3, 0, 2, 2 );
        Patch p( simpleImage, roi, NORMALIZE );
        p.dump();
        std::cout << "Mean: " << p.mean() << std::endl;
        std::cout << "Variance: " << p.variance() << std::endl;
		
	} catch( cvt::Exception e ) {
		std::cerr << e.what() << std::endl;
		return 1;
	}
}
