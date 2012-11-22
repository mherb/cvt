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
#include <cvt/vision/features/FeatureSet.h>

namespace cvt {

	class FeatureDetector
	{
		public:
			virtual ~FeatureDetector() {}
			virtual void detect( FeatureSet& set, const Image& img ) = 0;
			virtual void detect( FeatureSet& set, const ImagePyramid& imgpyr) = 0;
	};
}

#endif
