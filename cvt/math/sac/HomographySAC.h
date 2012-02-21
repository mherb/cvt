/*
			CVT - Computer Vision Tools Library

 	 Copyright (c) 2012, Philipp Heise, Sebastian Klose

 	THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 	KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 	IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 	PARTICULAR PURPOSE.
 */
/*
 * File:   HomographySAC.h
 * Author: sebi
 *
 * Created on July 20, 2011, 10:05 AM
 */

#ifndef CVT_HOMOGRAPHYSAC_H
#define	CVT_HOMOGRAPHYSAC_H

#include <cvt/math/sac/SampleConsensusModel.h>
#include <cvt/math/Matrix.h>
#include <cvt/vision/FeatureMatch.h>
#include <cvt/geom/PointSet.h>


#include <Eigen/SVD>


namespace cvt
{
    class HomographySAC;

    template<>
    struct SACModelTraits<HomographySAC>
    {
        typedef Matrix3f   ResultType;
        typedef float      DistanceType;
    };

    class HomographySAC : public SampleConsensusModel<HomographySAC>
{
      public:
        typedef SACModelTraits<HomographySAC>::ResultType       ResultType;
        typedef SACModelTraits<HomographySAC>::DistanceType     DistanceType;


        HomographySAC( const std::vector<FeatureMatch>& matches ) : _matches( matches )
        {
        }

        size_t size() const { return _matches.size(); }

        size_t minSampleSize() const
        {
            // minimum 4 correspondences to estimate
            // a Homography
            return 4;
        }

        ResultType estimate( const std::vector<size_t> & sampleIndices ) const;

        ResultType refine( const std::vector<size_t> & inlierIndices ) const;

        void inliers( std::vector<size_t> & inlierIndices, const ResultType & estimate, const DistanceType maxDistance ) const;

      private:
        const std::vector<FeatureMatch>&    _matches;
    };

    HomographySAC::ResultType HomographySAC::estimate( const std::vector<size_t> & sampleIndices ) const
    {
        PointSet2f set0, set1;
        for( size_t i = 0; i < sampleIndices.size(); i++ ){
            set0.add( _matches[ sampleIndices[ i ] ].feature0->pt );
            set1.add( _matches[ sampleIndices[ i ] ].feature1->pt );
        }

        return set0.alignPerspective( set1 );
    }

    HomographySAC::ResultType HomographySAC::refine( const std::vector<size_t> & inlierIndices ) const
    {
        // TODO: would be nicer, to use estimate, to get a linear estimate,
        //       and then refine it iteratively using GN or LM e.g.
        return estimate( inlierIndices );
    }

    void HomographySAC::inliers( std::vector<size_t> & inlierIndices,
                                 const ResultType & estimate,
                                 const DistanceType maxDistance ) const
    {
        // reproject all matches and compute the inliers
        Vector2f pPrime;
        for( size_t i = 0; i < _matches.size(); i++ ){
            // calc p' = estimate * p
            pPrime = estimate *  _matches[ i ].feature0->pt;

            if( ( pPrime - _matches[ i ].feature1->pt ).length() < maxDistance )
                inlierIndices.push_back( i );
        }
    }
}

#endif

