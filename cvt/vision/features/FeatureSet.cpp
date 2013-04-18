/*
            CVT - Computer Vision Tools Library

     Copyright (c) 2012, Philipp Heise, Sebastian Klose

    THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
    KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
    IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
    PARTICULAR PURPOSE.
 */

#include <cvt/vision/features/FeatureSet.h>
#include <cvt/vision/features/NMSFilter.h>
#include <algorithm>

namespace cvt {
	void FeatureSet::filterNMS( int radius, bool sort )
	{
		if( sort) {
			CmpPos cmp;
			std::sort( _features.begin(), _features.end(), cmp );
		}

		NMSFilter nms( _features );
		nms.filter( radius );
	}


	void FeatureSet::filterANMS( int radius, float threshold, bool sort )
	{
		if( sort) {
			CmpPos cmp;
			std::sort( _features.begin(), _features.end(), cmp );
		}

	}

	void FeatureSet::filterBest( size_t n, bool sort )
	{
		if( sort ) {
			CmpScore cmp;
			std::sort( _features.begin(), _features.end(), cmp );
		}
		if( _features.size() > n )
			_features.resize( n );
	}


}
