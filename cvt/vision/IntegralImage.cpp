#include <cvt/vision/IntegralImage.h>
#include <cvt/util/Time.h>
#include <cvt/util/CVTTest.h>
#include <cvt/io/Resources.h>

#define DUMP(v) std::cout << #v << " = " << v << std::endl;

namespace cvt
{
	IntegralImage::IntegralImage( const Image & img, IntegralImageFlags flags ) : _img( img), _sum( 0 ), _sqrSum( 0 ), _flags( flags )
	{	
        if( flags & SUMMED_AREA ){
            _sum = new Image( img.width(), img.height(), IFormat::floatEquivalent( img.format() ), img.memType() );
            img.integralImage( *_sum );
        }
        if( flags & SQUARED_SUMMED_AREA ){
            _sqrSum = new Image( img.width(), img.height(), IFormat::floatEquivalent( img.format() ), img.memType() );
            img.squaredIntegralImage( *_sqrSum );
        }
	}
    
    IntegralImage::~IntegralImage()
    {
        if( _sum )
            delete _sum;
        if( _sqrSum )
            delete _sqrSum;
    }
	   
    float IntegralImage::area( const Recti & r ) const
    {
        if( !_flags & SUMMED_AREA )
            throw CVTException( "Summed Area Table is not computed -> cannot calculate area" );
        
        return IntegralImage::area( *_sum, r );
    }
    
    float IntegralImage::sqrArea( const Recti & r ) const
    {
        if( !_flags & SQUARED_SUMMED_AREA )
            throw CVTException( "Squared Summed Area Table is not computed -> cannot calculate area" );
        
        return IntegralImage::area( *_sqrSum, r );
    }
    
    float IntegralImage::ncc( const IntegralImage & other, const Recti & rOther, const Vector2i & pos ) const
    {
        if( !_flags & SUMMED_AREA || !_flags & SQUARED_SUMMED_AREA ||
            !other.flags() & SUMMED_AREA ||!other.flags() & SQUARED_SUMMED_AREA ){
            throw CVTException( "NCC needs SUMMED_AREA and SQUARED_SUMMED_AREA" );
        }
        
        // corresponding rect in img:
        Recti iRect( pos.x, pos.y, rOther.width, rOther.height );
                
        float sumI = IntegralImage::area( *_sum, iRect );
        float ssumI = IntegralImage::area( *_sqrSum, iRect );
        
        float sumO = IntegralImage::area( other.sumImage(), rOther );
        float ssumO = IntegralImage::area( other.sqrSumImage(), rOther );
        float size = rOther.width * rOther.height;
        
        float meanP = sumO / size;
        float sigmaPSigmaI = Math::invSqrt( ( ssumO - Math::sqr( sumO ) / size ) * ( ssumI - Math::sqr( sumI ) / size ) );

                       
        // calc SUM( I_i * P_i ) 
        size_t istride, pstride;
        const uint8_t* i = _img.map( &istride );
        const uint8_t* p = other.image().map( &pstride );
     
        const uint8_t* iPtr = i + iRect.y * istride + iRect.x;
        const uint8_t* pPtr = p + rOther.y * pstride + rOther.x;        
        float mulSum = 0;
        for( int y = 0; y < iRect.height; y++ ){
            for( int x = 0; x < iRect.width; x++ ){
                mulSum += ( ( float )iPtr[ x ] * ( float )pPtr[ x ] );
            } 
            iPtr += istride;
            pPtr += pstride;
        }
        _img.unmap( i );
        other.image().unmap( p );
                
        return ( mulSum - meanP * sumI ) * sigmaPSigmaI;
    }    
    
    /*********************************************************/
    
    static float areaTest( const Image & img, const Recti & r )
    {
        size_t stride;
        const uint8_t * p = img.map( &stride );
        float sum = 0;
        for( int y = 0; y < r.height && y < ( int )img.height(); y++ ){
            for( int x = 0; x < r.width && x < ( int )img.width(); x++ ){
                sum += p[ ( r.y + y ) * stride + r.x + x ];
            }
        }
        img.unmap( p );
        return sum;
    }      
    
    static float squaredAreaTest( const Image & img, const Recti & r )
    {
        size_t stride;
        const uint8_t * p = img.map( &stride );
        float sum = 0;
        for( int y = 0; y < r.height && y < ( int )img.height(); y++ ){
            for( int x = 0; x < r.width && x < ( int )img.width(); x++ ){
                sum += Math::sqr( ( float )p[ ( r.y + y ) * stride + r.x + x ] );
            }
        }
        img.unmap( p );
        return sum;
    }   
   
    static void _genImage( Image & img )
    {
        size_t s0;
        uint8_t * p0 = img.map( &s0 );
        uint8_t * p = p0;
        
        uint8_t v = 1;
        for( size_t i = 0; i < img.height(); i++ ){
            for( size_t k = 0; k < img.width(); k++ ){            
                p0[ k ] = v++;
            }
            p0+=s0;
        }
        img.unmap( p );
    }
    
    template <typename T>
    static void _dumpImage( Image & img )
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
    
    static void _tableSum( Image & img, Image & sumImage ){
        size_t iStride, sStride;
        uint8_t * ip = img.map( &iStride );
        float * is = sumImage.map<float>( &sStride );
                
        for( size_t y = 0; y < img.height(); y++ ){
            float rowSum = 0;
            for( size_t x = 0; x < img.width(); x++ ){
                rowSum += ip[ y * iStride + x ];
                if( y > 0 )
                    is[ y * sStride + x ] = rowSum + is[ ( y-1 ) * sStride + x ];
                else
                    is[ y * sStride + x ] = rowSum;
            }
        }        
        img.unmap( ip );
        sumImage.unmap( is );
    }
    
    static void _tableSqrSum( Image & img, Image & sumImage ){
        size_t iStride, sStride;
        uint8_t * ip = img.map( &iStride );
        float * is = sumImage.map<float>( &sStride );
        
        for( size_t y = 0; y < img.height(); y++ ){
            float rowSum = 0.0f;
            for( size_t x = 0; x < img.width(); x++ ){
                rowSum += Math::sqr( ( float )ip[ y * iStride + x ] );
                if( y > 0 )
                    is[ y * sStride + x ] = rowSum + is[ ( y - 1 ) * sStride + x ];
                else
                    is[ y * sStride + x ] = rowSum;
            }
        }        
        img.unmap( ip );
        sumImage.unmap( is );
    }
    
    static bool _compare( Image & gt, Image & comp )
    {
        size_t iStride, sStride;
        float * ip = gt.map<float>( &iStride );
        float * is = comp.map<float>( &sStride );
        
        bool ret = true;
        for( size_t y = 0; y < gt.height(); y++ ){
            for( size_t x = 0; x < gt.width(); x++ ){
                
                if( Math::abs( ip[ y * iStride + x ] - is[ y * sStride + x ] ) > Math::EPSILONF ){
                    std::cout << "Error: Pos = " << y << ", " << x << ": GT = " << ip[ y * iStride + x ] << " Computed: " << is[ y * iStride + x ] << std::endl; 
                    ret = false;
                    break;
                }
            }
            if( !ret )
                break;
        }
        gt.unmap( ip );
        comp.unmap( is );
        return ret;
    }
    
    static bool _testRectSum( Image & img, Image & sum, Recti & rect )
    {
        float iiArea;
        iiArea = IntegralImage::area( sum, rect );
        
        float iArea;
        iArea = areaTest( img, rect );
        
        //std::cout << "Area Test -> Rect = " << rect << " " << iiArea << " ?== " << iArea << std::endl;        
        return Math::abs( iArea - iiArea ) < Math::EPSILONF;
    }
    
    static bool _testSqrRectSum( Image & img, Image & sum, Recti & rect )
    {
        float iiArea;
        iiArea = IntegralImage::area( sum, rect );
        
        float iArea;
        iArea = squaredAreaTest( img, rect );        
        
        //std::cout << "Area Test -> Rect = " << rect << " " << iiArea << " ?== " << iArea << std::endl;        
        return Math::abs( iArea - iiArea ) < Math::EPSILONF;
    }
    
    BEGIN_CVTTEST( integralImage )
    
    Image img( 20, 20, IFormat::GRAY_UINT8 );
    _genImage( img );
       
    IntegralImage ii( img, ( SUMMED_AREA | SQUARED_SUMMED_AREA ) );
   
    Image gtSum( img.width(), img.height(), IFormat::GRAY_FLOAT );
    _tableSum( img, gtSum );
    Image gtSqrSum( img.width(), img.height(), IFormat::GRAY_FLOAT );
    _tableSqrSum( img, gtSqrSum );    
    
    bool result = true;    
    bool test;
    
    test = _compare( gtSum, ii.sumImage() );
    if( !test ){
        std::cout << "GT: \n";
        _dumpImage<float>( gtSum );
        std::cout << "\nIntImage: \n";
        _dumpImage<float>( ii.sumImage() );
    }
    CVTTEST_PRINT("::summedImage( ... )",  test );
    result &= test;
    
    test = _compare( gtSqrSum, ii.sqrSumImage() );
        if( !test ){
            std::cout << "GT: \n";
            _dumpImage<float>( gtSqrSum );
            std::cout << "\nIntImage: \n";
            _dumpImage<float>( ii.sqrSumImage() );
        }
    CVTTEST_PRINT("::sqrSummedImage( ... )",  test );
    result &= test;    
          
    Recti rect( 0, 0, 4, 4 );    
    test = true;
    for( int y = 0; y < 10; y++ ){
        rect.y = y;
        for( int x = 0; x < 10; x++ ){
            rect.x = x;
            test &= _testRectSum( img, ii.sumImage(), rect );
        }
    }    
    CVTTEST_PRINT("::area( ... )",  test );
    result &= test;
    
    test = true;
    for( int y = 0; y < 10; y++ ){
        rect.y = y;
        for( int x = 0; x < 10; x++ ){
            rect.x = x;
            test &= _testSqrRectSum( img, ii.sqrSumImage(), rect );
        }
    }
    CVTTEST_PRINT("::squaredArea( ... )", test );
    result &= test;
    
    return result;
    
    END_CVTTEST

}
