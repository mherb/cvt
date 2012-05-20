/*
			CVT - Computer Vision Tools Library

 	 Copyright (c) 2012, Philipp Heise, Sebastian Klose

 	THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 	KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 	IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 	PARTICULAR PURPOSE.
 */
/*
 * File:   RANSAC.h
 * Author: sebi, heise
 *
 * Created on July 19, 2011, 1:34 PM
 */

#ifndef CVT_RANSAC_H
#define	CVT_RANSAC_H

#include <cvt/math/Math.h>
#include <cvt/math/sac/SampleConsensusModel.h>

namespace cvt
{
    template <class Model>
    class RANSAC
    {
      public:
        typedef typename Model::ResultType   ResultType;
        typedef typename Model::DistanceType DistanceType;

        RANSAC( SampleConsensusModel<Model> & model,
                DistanceType maxDistance,
                float outlierProb = 0.05f ) :
            _model( model ), _maxDistance( maxDistance ), _outlierProb( outlierProb )
        {
        }

        ResultType estimate( size_t maxIter = 0 );

      private:
        SampleConsensusModel<Model>&  _model;

        DistanceType                  _maxDistance;
        float                         _outlierProb;

        void randomSamples( std::vector<size_t> & indices );
    };

    template<class Model>
    inline typename RANSAC<Model>::ResultType RANSAC<Model>::estimate( size_t maxIter )
    {
        size_t n = ( size_t )-1;
        size_t samples = 0;

        ResultType result;

        std::vector<size_t> indices;
        std::vector<size_t> inliers;

        std::vector<size_t> bestIndices;
        size_t numBest = 0;

        if( maxIter )
            n = maxIter;

        while( n > samples ){
            randomSamples( indices );
            result = _model.estimate( indices );

            _model.inliers( inliers, result, _maxDistance );

            if( inliers.size() > numBest ){
                numBest = inliers.size();
                bestIndices = indices;

                float epsilon = 1.0f - ( float )inliers.size() / ( float )_model.size();

                size_t newn = Math::log( _outlierProb ) / Math::log( 1.0f - Math::pow( 1.0f - epsilon, ( float )_model.minSampleSize() ) );

                if( maxIter > 0 ){
					if( newn < maxIter ){
						n = newn;
					}
				} else {
                    n = newn;
				}
            }


            samples++;
        }

        result = _model.estimate( bestIndices );
        _model.inliers( inliers, result, _maxDistance );

        return _model.refine( result, inliers );
    }

    template<class Model>
    inline void RANSAC<Model>::randomSamples( std::vector<size_t> & indices )
	{
        indices.clear();

		size_t idx;
		while( indices.size() < _model.minSampleSize() ){
			idx = Math::rand( 0, _model.size() - 1 );

            bool insert = true;
            for( size_t i = 0; i < indices.size(); i++ ){
                if( idx == indices[ i ] ){
                    insert = false;
                    break;
                }
            }

            if( insert ){
                indices.push_back( idx );
			}
		}
	}
}

#endif	/* RANSAC_H */

