/*
			CVT - Computer Vision Tools Library

 	 Copyright (c) 2012, Philipp Heise, Sebastian Klose

 	THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 	KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 	IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 	PARTICULAR PURPOSE.
 */
/*
 * File:   FeatureMatch.h
 * Author: Sebastian Klose
 *
 * Created on June 28, 2011
 */

#ifndef CVT_FEATUREMATCH_H
#define	CVT_FEATUREMATCH_H

#include <cvt/vision/features/Feature.h>

namespace cvt {

    struct FeatureMatch {

        FeatureMatch() :
            feature0( 0 ), feature1( 0 ), distance( -1.0f )
        {
        }

        FeatureMatch( const FeatureMatch & other ) :
            feature0( other.feature0 ),
            feature1( other.feature1 ),
            distance( other.distance )
        {
        }

        FeatureMatch& operator=( const FeatureMatch& other )
        {
            feature0 = other.feature0;
            feature1 = other.feature1;
            distance = other.distance;
            return *this;
        }

        const Feature* 	feature0;
        const Feature* 	feature1;
        float			distance;
    } ;

}

#endif

