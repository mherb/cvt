/*
            CVT - Computer Vision Tools Library

     Copyright (c) 2012, Philipp Heise, Sebastian Klose

    THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
    KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
    IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
    PARTICULAR PURPOSE.
 */
#ifndef CVT_FEATUREDETECTOR_H
#define CVT_FEATUREDETECTOR_H

#include <cvt/gfx/Image.h>
#include <cvt/vision/ImagePyramid.h>

namespace cvt {

	class FeatureDetector
	{
		public:
			virtual void detect( const Image& img, FeatureSet& set ) const = 0;
			virtual void detect( const ImagePyramid& imgpyr, FeatureSet& set ) const = 0;
	};
}

#endif
