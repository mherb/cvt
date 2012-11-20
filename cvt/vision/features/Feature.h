/*
            CVT - Computer Vision Tools Library

     Copyright (c) 2012, Philipp Heise, Sebastian Klose

    THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
    KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
    IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
    PARTICULAR PURPOSE.
 */
#ifndef CVT_FEATURE_H
#define CVT_FEATURE_H

#include <cvt/math/Vector.h>

namespace cvt {

    struct Feature {
        Feature( float x = 0, float y = 0, float angle = 0.0f, size_t octave = 0, float score = 0.0f );

        Vector2f pt;
        float angle;
        size_t octave;
        float score;
    };

    inline Feature::Feature( float x, float y, float a, size_t o, float sc ) : pt( x, y ), angle( a ), octave( o ), score( sc )
    {
    }



}

#endif
