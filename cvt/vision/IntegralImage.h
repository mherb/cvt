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
            float   sqrArea( const Recti & r ) const;
        
            /**
             * normalized cross correlation:
             * calculates the normalized cross correlation of the Rectangle rOther from other
             * with pos in this image
             */
            float   ncc( const IntegralImage & other, const Recti & rOther, const Vector2i & pos ) const;
        
            Image & sumImage() const { return *_sum; };
            Image & sqrSumImage() const { return *_sqrSum; };  
            IntegralImageFlags flags() const { return _flags; };        
            const Image & image() const { return _img; };
        
            static inline float area( const Image & img, const Recti & r ); 
        
        private:
            const Image &       _img;
            Image*              _sum;
            Image*              _sqrSum;
            IntegralImageFlags  _flags;

	};
    
    inline float IntegralImage::area( const Image & img, const Recti & r )
    {
        size_t stride;
        const float * p = img.map<float>( &stride );
        
        int xOffset, yOffset;
        
        yOffset = r.y - 1;
        xOffset = r.x - 1;
        
        float sum = p[ ( yOffset + r.height ) * stride + xOffset + r.width ];        
        
        if( yOffset >= 0 ){
            if( xOffset >= 0 ){ // +a
                sum += p[ yOffset * stride + xOffset ];
                sum -= p[ ( yOffset + r.height ) * stride + xOffset ];
            }
            
            xOffset = Math::min( xOffset + r.width, ( int )img.width() - 1 );            
            sum -= p[ yOffset * stride + xOffset ];
        } else {
            yOffset = yOffset + r.height;
            if( xOffset >= 0 ){ // -c
                sum -= p[ yOffset * stride + xOffset ];            
            }
        }
        
        img.unmap( p );
        return sum;
    }
}

#endif
