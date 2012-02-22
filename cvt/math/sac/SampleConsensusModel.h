/*
			CVT - Computer Vision Tools Library

 	 Copyright (c) 2012, Philipp Heise, Sebastian Klose

 	THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 	KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 	IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 	PARTICULAR PURPOSE.
 */
/*
 * File:   SampleConsensusModel.h
 * Author: sebi, heise
 *
 * Created on July 19, 2011, 1:40 PM
 */

#ifndef CVT_SAMPLECONSENSUSMODEL_H
#define	CVT_SAMPLECONSENSUSMODEL_H

#include <vector>

namespace cvt
{
    /**
     * SampleConsensusModelTraits:
     * -> typedefs on ResultType and DistanceType
     */
    template<class T>
    struct SACModelTraits;

    template <typename Derived>
    class SampleConsensusModel
    {
      public:
        typedef typename SACModelTraits<Derived>::ResultType    ResultType;
        typedef typename SACModelTraits<Derived>::DistanceType  DistanceType;

        size_t size() const
        {
            return ( ( Derived *)this )->size();
        }

        size_t minSampleSize() const
        {
            return ( ( Derived *)this )->minSampleSize();
        }

        ResultType estimate( const std::vector<size_t> & sampleIndices ) const
        {
            return ( ( Derived *)this )->estimate( sampleIndices );
        }

        ResultType refine( const std::vector<size_t> & inliers  ) const
        {
            return ( ( Derived *)this )->refine( inliers );
        }

        void inliers( std::vector<size_t> & sampleIndices,
                      const ResultType & estimate,
                      const DistanceType maxDistance ) const
        {
            ( ( Derived *)this )->inliers( sampleIndices, estimate, maxDistance );
        }
    };
}

#endif

