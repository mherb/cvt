#ifndef CVT_INTEGRAL_IMAGE_H
#define CVT_INTEGRAL_IMAGE_H

#include <cvt/gfx/Image.h>

namespace cvt
{
	class IntegralImage
	{
		public:
            IntegralImage( const Image & img );
            ~IntegralImage();

            float   area( const Recti & r ) const;
        
            Image & sumImage() { return _sum; };
            Image & sqrSumImage() { return *_sqrSum; };
        
        private:
            Image   _sum;
            Image*  _sqrSum;

	};
}

#endif
