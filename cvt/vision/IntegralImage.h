#ifndef CVT_INTEGRAL_IMAGE_H
#define CVT_INTEGRAL_IMAGE_H

#include <cvt/gfx/Image.h>
#include <cvt/util/Flags.h>

namespace cvt
{
    enum IntImgFlagTypes {
		SUMMED_AREA = ( 1 << 0 ),
		SQUARED_SUMMED_AREA = ( 1 << 1 )
	};
    
	CVT_ENUM_TO_FLAGS( IntImgFlagTypes, IntegralImageFlags )
    
	class IntegralImage
	{
		public:
            IntegralImage( const Image & img, IntegralImageFlags flags = SUMMED_AREA );
            ~IntegralImage();

            float   area( const Recti & r ) const;
        
            static inline float area( const Image & img, const Recti & r );
        
            Image & sumImage() { return *_sum; };
            Image & sqrSumImage() { return *_sqrSum; };  
        
        
        private:
            Image*  _sum;
            Image*  _sqrSum;

	};
    
    inline float IntegralImage::area( const Image & img, const Recti & r )
    {
        size_t stride;
        const float * p = img.map<float>( &stride );
        
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
        
        img.unmap( p );
		return sum;
    }
}

#endif
