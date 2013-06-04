/*
         CVT - Computer Vision Tools Library

    Copyright (c) 2012, Philipp Heise, Sebastian Klose

   THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
   KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
   IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
   PARTICULAR PURPOSE.
 */
#ifndef CVT_FEATURE_TRACKING_H
#define CVT_FEATURE_TRACKING_H

#include <vector>
#include <cvt/geom/PointSet.h>
#include <cvt/util/Signal.h>
#include <cvt/vision/features/FeatureDetector.h>
#include <cvt/vision/features/FeatureDescriptor.h>
#include <cvt/vision/features/FeatureDescriptorExtractor.h>
#include <cvt/gfx/Image.h>

namespace cvt
{
    class FeatureTracking
    {
        public:
			FeatureTracking();
			~FeatureTracking();

			void track( std::vector<size_t> &tracked,
						const Image& currGray,
						const std::vector<FeatureDescriptor*>& descriptors );


		private:
            bool checkFeatureSAD( const Vector2f& p0, const Vector2f& p1, const Image & i0, const Image & i1 ) const;
    };
}

#endif

