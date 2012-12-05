/*
			CVT - Computer Vision Tools Library

 	 Copyright (c) 2012, Philipp Heise, Sebastian Klose

 	THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 	KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 	IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 	PARTICULAR PURPOSE.
 */
#ifndef CVT_THRESHOLD_H
#define CVT_THRESHOLD_H

namespace cvt {
	class Image;

	class IThreshold
	{
		public:
			static void threshold( Image& dst, const Image& src, float t );
			static void thresholdAdaptive( Image& dst, const Image& src, const Image& boxfiltered, float t );

		private:
			IThreshold() {}
			IThreshold( const IThreshold& ) {}
	};

}

#endif
