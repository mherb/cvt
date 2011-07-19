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
    template <class Derived, class ResultType, typename DistanceType>
    class SampleConsensusModel
    {
      public:
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

