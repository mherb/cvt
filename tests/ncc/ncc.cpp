#include <iostream>
#include <iomanip>

#include <cvt/vision/IntegralImage.h>
#include <cvt/io/Resources.h>


using namespace cvt;

void genImage( Image & img )
{
    size_t stride;
    
    uint8_t * p, *ptr;
    p = ptr = img.map( &stride );
    size_t h = img.height();
    uint8_t i = 1;    
    while( h-- ){
        for( size_t x = 0; x < img.width(); x++ ){
            p[ x ] = i++;
        }
        p +=stride;
    }
    img.unmap( ptr );
}

template <typename T>
void dumpImage( Image & img )
{
    size_t stride;
    
    T * p, *ptr;
    p = ptr = img.map<T>( &stride );
    size_t h = img.height();
    while( h-- ){
        for( size_t x = 0; x < img.width(); x++ ){            
            std::cout << ( int )p[ x ] << "  ";
        }
        std::cout << std::endl;
        p +=stride;
    }
    img.unmap( ptr );
}

float sqrSum( Image & img, Recti & r )
{
    size_t stride;
    uint8_t * p = img.map( &stride );
    
    uint8_t * curr = p + r.y * stride + r.x;
    float sum = 0;
    for( int i = 0; i < r.height; i++ ){        
        for( int k = 0; k < r.width; k++ ){
            sum += Math::sqr( ( float )( curr[ k ] ) );
        }
        curr += stride;
    }
    return sum;
}

int main( void )
{
	Resources resources;

	Image img( resources.find( "lena_g.png" ) );
    //Image img( 20, 20, IFormat::GRAY_UINT8 );
    genImage( img );
    
    //dumpImage<uint8_t>( img );

	IntegralImage intImage( img, ( SUMMED_AREA | SQUARED_SUMMED_AREA ) );
    
    //std::cout << "\nIntegral Image" << std::endl;
    //dumpImage<float>( intImage.sumImage() );
    
	Recti r0( 40, 50, 80, 90 );    
	Vector2i p( r0.x, r0.y );
    
	float ncc;     
    for( size_t i = 0; i < 1; i++ ){
        ncc = intImage.ncc( intImage, r0, p );
        std::cout << "NCC: " << ncc << std::endl;        
        p.x++;
    }

	return 0;
}
