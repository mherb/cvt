/*
			CVT - Computer Vision Tools Library

 	 Copyright (c) 2012, Philipp Heise, Sebastian Klose

 	THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 	KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 	IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 	PARTICULAR PURPOSE.
 */
#ifndef CVT_IFILL_H
#define CVT_IFILL_H

namespace cvt
{
	class Image;
	class Color;

	class IFill {
		public:
			static void fill( Image& dst, const Color& c );

		private:
			IFill() {}
			IFill( const IFill& ) {}
	};

}

#endif
