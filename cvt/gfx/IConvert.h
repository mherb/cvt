/*
			CVT - Computer Vision Tools Library

 	 Copyright (c) 2012, Philipp Heise, Sebastian Klose

 	THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 	KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 	IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 	PARTICULAR PURPOSE.
 */
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

			static const IConvert & instance();

		private:
			IConvert();

			static IConvert * _instance;
			ConversionFunction * _convertFuncs;

			void initTable();
	};
}

#endif
