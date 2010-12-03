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
			void convert( Image & dst, const Image & src ) const;

			static const IConvert & instance();

		private:
			IConvert();

			static IConvert * _instance;
			ConversionFunction * _convertFuncs;

			void initTable();
	};
}

#endif
