/*
            CVT - Computer Vision Tools Library

     Copyright (c) 2012, Philipp Heise, Sebastian Klose

    THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
    KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
    IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
    PARTICULAR PURPOSE.
 */
#ifndef CVT_FEATURESET_H
#define CVT_FEATURESET_H

#include <cvt/gfx/Image.h>

namespace cvt {

	class FeatureSet
	{
		public:
			virtual void add( const Feature& feature ) const = 0;
			inline void operator()( const Feature& feature ) { add( feature ); }

			virtual size_t size() const = 0;
			virtual Feature& operator[]( size_t i ) = 0;
			virtual const Feature& operator[]( size_t i ) const = 0;

			virtual void filter() {};
	};
}

#endif
