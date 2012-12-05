/*
			CVT - Computer Vision Tools Library

 	 Copyright (c) 2012, Philipp Heise, Sebastian Klose

 	THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 	KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 	IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 	PARTICULAR PURPOSE.
 */
#ifndef CVT_IMORPHOLOGICAL_H
#define CVT_IMORPHOLOGICAL_H

namespace cvt
{
	class Image;

	class IMorphological
	{
		public:
			static void dilate( Image& dst, const Image& src, size_t radius );
			static void erode( Image& dst, const Image& src, size_t radius );
//			static void open( Image& dst, const Image& src, size_t radius );
//			static void close( Image& dst, const Image& src, size_t radius );
		private:
			IMorphological() {}
			IMorphological( const IMorphological& ) {}
	};
}

#endif
