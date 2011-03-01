#ifndef CVT_INTEGRAL_IMAGE_H
#define CVT_INTEGRAL_IMAGE_H

#include <cvt/gfx/Image.h>

namespace cvt
{
	class IntegralImage : public Image
	{
		public:
            IntegralImage( const Image & in, IAllocatorType memType = IALLOCATOR_MEM );
			IntegralImage( const IntegralImage & in, IAllocatorType memType = IALLOCATOR_MEM );

			void update( const Image & input );
            float area( const Recti & r ) const;

	};
}

#endif
