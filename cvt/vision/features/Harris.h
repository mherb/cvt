/*
			CVT - Computer Vision Tools Library

	 Copyright (c) 2012, Philipp Heise, Sebastian Klose

	THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
	KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
	IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
	PARTICULAR PURPOSE.
 */
#ifndef CVT_HARRIS_H
#define CVT_HARRIS_H

#include <cvt/vision/features/FeatureDetector.h>
#include <cvt/gfx/Image.h>


namespace cvt
{
	class Harris : public FeatureDetector
	{
		public:
			Harris( float threshold = 30, size_t border = 3 );
			~Harris();

			void detect( FeatureSet& features, const Image& image );
			void detect( FeatureSet& features, const ImagePyramid& image );

			void setThreshold( float threshold ) { _threshold = threshold; }
			float threshold() const	{ return _threshold; }

			void setBorder( size_t border )	{ _border = border; }
			size_t border() const	{ return _border; }

		private:
			float		_threshold;
            size_t		_border;
	};

}

#endif
