#include <cvt/vision/IntegralImage.h>
#include <cvt/util/Time.h>
#include <cvt/util/CVTTest.h>
#include <cvt/io/Resources.h>


namespace cvt
{
	IntegralImage::IntegralImage( const Image & img ) : _sqrSum( 0 )
	{	
        img.integralImage( _sum );
	}
    
    IntegralImage::~IntegralImage()
    {
        if( _sqrSum )
            delete _sqrSum;
    }
	   
    float IntegralImage::area( const Recti & r ) const
    {
        size_t stride;
        const float * p = _sum.map<float>( &stride );
        
        float sum = 0;
        int xOffset, yOffset;
        
        yOffset = r.y - 1;        
        if( yOffset >= 0 ){
            xOffset = r.x - 1;
            
            if( xOffset >= 0 ) // +a
                sum += p[ yOffset * stride + xOffset ];
            
            xOffset += r.width;
            if( xOffset >= 0 ) // -b
                sum -= p[ yOffset * stride + xOffset ];
        }
        
        yOffset = yOffset + r.height;
        xOffset = r.x - 1;
        if( r.x >= 0 ) // -c
            sum -= p[ yOffset * stride + xOffset ];
        
        xOffset += r.width;
        sum += p[ yOffset * stride + xOffset ];        
        
        _sum.unmap( p );
        
        return sum;
    }
    
    static float areaTest( const Image & img, const Recti & r )
    {
        size_t stride;
        const uint8_t * p = img.map( &stride );
        float sum = 0;
        for( int y = 0; y < r.height; y++ ){
            for( int x = 0; x < r.width; x++ ){
                sum += p[ ( r.y + y ) * stride + r.x + x ];
            }
        }
        img.unmap( p );
        return sum;
    }
    
    static void _dump( const Image & img, const Image & integral )
    {
        size_t s0, s1;
        const uint8_t * p0 = img.map( &s0 );
        const float * p1 = integral.map<float>( &s1 );
        
        std::cout << "Input: " << std::endl;
        for( size_t i = 0; i < img.height(); i++ ){
            for( size_t k = 0; k < img.width(); k++ ){            
                std::cout << ( int )p0[ i * s0 + k ] << " ";
            }
            std::cout << std::endl;
        }
        
        std::cout << "\nResult: " << std::endl;
        for( size_t i = 0; i < img.height(); i++ ){
            for( size_t k = 0; k < img.width(); k++ ){            
                std::cout << p1[ i * s1 + k ] << " ";
            }
            std::cout << std::endl;
        }
        
        img.unmap( p0 );
        integral.unmap( p1 );
    }
    
    static void _genImage( Image & img )
    {
        size_t s0;
        uint8_t * p0 = img.map( &s0 );
        uint8_t * p = p0;
        
        uint8_t v = 0;
        for( size_t i = 0; i < img.height(); i++ ){
            for( size_t k = 0; k < img.width(); k++ ){            
                p0[ k ] = v++;
            }
            p0+=s0;
        }
        img.unmap( p );
    }
    
    BEGIN_CVTTEST( integralImage )
    
    Image img( 30, 40, IFormat::GRAY_UINT8 );
    _genImage( img );
       
    IntegralImage ii( img );
    
    _dump( img, ii.sumImage() );
    
    std::cout << "\nSQUARED SUM\n" << std::endl;
    Image iSum, iSSum;
    img.integralImage( iSum );
    img.squaredIntegralImage( iSSum );
    _dump( img, iSSum );
        
    Recti rect( 10, 20, 10, 15 );    
    float iiArea;
    iiArea = ii.area( rect );

    
        
    float iArea;
    iArea = areaTest( img, rect );
        
    bool result = ( iArea == iiArea );
    CVTTEST_PRINT("::area( ... )",  result );
    
    
    return true;
    
    END_CVTTEST

}
