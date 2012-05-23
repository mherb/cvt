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

#include <cvt/gfx/IFormat.h>
#include <cvt/util/Flags.h>
#include <cvt/util/Exception.h>

namespace cvt
{
	class Image;

	enum IConvertFlagTypes {
		ICONVERT_DEBAYER_LINEAR = ( 1 << 0 ),
		ICONVERT_DEBAYER_HQLINEAR = ( 1 << 1 )
		/*
		 TODO: gamma treatment

		 ICONVERT_COLOR_SRGB_TO_LINEAR = ( 1 << 2 ),
		 ICONVERT_COLOR_LINEAR_TO_LINEAR = ( 1 << 3 ),
		 ICONVERT_COLOR_LINEAR_TO_SRGB = ( 1 << 4 ),
		 */
	};

	CVT_ENUM_TO_FLAGS( IConvertFlagTypes, IConvertFlags )

	typedef void (*ConversionFunction)( Image&, const Image&, IConvertFlags flags );

	class IConvert
	{
		public:
			/* conversion from source format to dst format */
			static void convert( Image& dst, const Image& src, IConvertFlags flags = ICONVERT_DEBAYER_LINEAR );

			static const IConvert & instance();

		private:
			IConvert();

			static IConvert * _instance;
			ConversionFunction * _convertFuncs;

			void initTable();
	};
}

#endif
