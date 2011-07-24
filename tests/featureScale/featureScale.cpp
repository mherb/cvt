#include <iostream>

#include <cvt/gfx/Image.h>
#include <cvt/vision/FAST.h>
#include <cvt/vision/Feature2D.h>
#include <cvt/vision/IntegralImage.h>

using namespace cvt;

void testFeatureScales( const Image & gray )
{
    FAST fast;
    fast.setThreshold( 35 );

    Image integral;
    gray.integralImage( integral );


    std::vector<Feature2Df> features;
    VectorFeature2DInserter<float> inserter( features );
    fast.extract( gray, inserter );

    Recti smallRect, bigRect;
    size_t offsetSmall = 3, offsetBig = 8;
    smallRect.height = 7;
    smallRect.width = 7;
    bigRect.width = 17;
    bigRect.height = 17;

    float avgScale = 0;
    size_t num = 0;
    for( size_t i = 0; i < features.size(); i++ ){
        const Feature2Df & f = features[ i ];
        if( f.pt.x > offsetBig && f.pt.x < gray.width() - offsetBig &&
            f.pt.y > offsetBig && f.pt.y < gray.height() - offsetBig ){
            smallRect.x = ( size_t )f.pt.x - offsetSmall;
            smallRect.y = ( size_t )f.pt.y - offsetSmall;
            bigRect.x = ( size_t )f.pt.x - offsetBig;
            bigRect.y = ( size_t )f.pt.y - offsetBig;
            float areaSmall = IntegralImage::area( integral, smallRect );
            float areaBig = IntegralImage::area( integral, bigRect );

            float scale = areaSmall / areaBig;
            avgScale += scale;
            num++;

            //std::cout << "Point " << i << ":\t" << scale << std::endl;
        }
    }
    std::cout << "Avg Scale: " << avgScale / num << std::endl;

}

int main( int argc, char* argv[] )
{
    if( argc < 2 ){
        std::cout << "Usage: " << argv[ 0 ] << " <image>" << std::endl;
        return 0;
    }

    Image img( argv[ 1 ] );
    Image gray;

    img.convert( gray, IFormat::GRAY_UINT8 );

    testFeatureScales( gray );

    Image scaled;
    IScaleFilterGauss filter;

    size_t width = gray.width() * 2;
    size_t height = gray.height() * 2;

    for( size_t i = 0; i < 4; i++ ){
        gray.scale( scaled, width, height, filter );
        testFeatureScales( scaled );
        width = width >> 1;
        height = height >> 1;
    }




	return 0;
}
