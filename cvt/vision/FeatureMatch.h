/*
 * File:   FeatureMatch.h
 * Author: Sebastian Klose
 *
 * Created on June 28, 2011
 */

#ifndef FEATUREMATCH_H
    #define	FEATUREMATCH_H

namespace cvt {

    struct FeatureMatch {
        int 	idx0;
        int 	idx1;
        float	distance;
    } ;

}

#endif	/* FEATUREMATCH_H */

