#include <cvt/vision/IntegralImage.h>
#include <cvt/util/Time.h>
#include <cvt/util/CVTTest.h>
#include <cvt/io/Resources.h>


namespace cvt
{
	IntegralImage::IntegralImage( const Image & input, IAllocatorType memType )
	{
		this->reallocate( input.width(), input.height(), IFormat::floatEquivalent( input.format() ), memType );
		this->update( input );
	}

	IntegralImage::IntegralImage( const IntegralImage & input, IAllocatorType memType ) : Image( input, memType )
	{
	}

	// calculate the parralel prefix sum if the input and store it in this
	void IntegralImage::update( const Image & input )
	{
        size_t inStride;
        size_t dstStride;
        
        float * out = map<float>( &dstStride );
        
        SIMD * simd = SIMD::instance();
                
        IFormatID fId = input.format().formatID;        
        switch ( fId ) {
            case IFORMAT_GRAY_UINT8:
            {
                const uint8_t * in = input.map( &inStride );
                simd->prefixSum1( out, dstStride, in, inStride, width(), height() );
                input.unmap( in );
            }
            break;
                
            default:
                this->unmap( out );
                throw CVTException( "IntegralImage not implemented for type: " + fId );
        }
        this->unmap( out );
	}
    
    float IntegralImage::area( const Recti & r ) const
    {
        size_t stride;
        const float * p = this->map<float>( &stride );
        
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
        
        this->unmap( p );
        
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
    
    BEGIN_CVTTEST( integralImage )
    
    Resources res;
    Image img( res.find( "lena_g.png" ) );
       
    IntegralImage ii( img );
       
    
    Recti rect( 10, 20, 23, 45 );    
    float iiArea;
    iiArea = ii.area( rect );

    
        
    float iArea;
    iArea = areaTest( img, rect );
        
    bool result = ( iArea == iiArea );
    CVTTEST_PRINT("::area( ... )",  result );
    
    
    return true;
    
    END_CVTTEST

}
