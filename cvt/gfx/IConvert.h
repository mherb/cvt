#ifndef CVT_ICONVERT
#define CVT_ICONVERT

#include <cvt/gfx/Image.h>
#include <cvt/gfx/IFormat.h>
#include <cvt/util/Exception.h>

namespace cvt
{
	typedef void (*ConversionFunction)( Image&, const Image& );
	
	class IConvert
	{
		public:
			/* conversion from source format to dst format */
			static void convert( Image & dst, const Image & src );
		
		private:			
			IConvert();
	};	
}

#endif