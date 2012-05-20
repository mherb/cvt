/*
			CVT - Computer Vision Tools Library

 	 Copyright (c) 2012, Philipp Heise, Sebastian Klose

 	THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 	KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 	IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 	PARTICULAR PURPOSE.
 */
/*
 * File:   EssentialSAC.h
 * Author: sebi
 *
 */

#ifndef CVT_ESSENTIALSAC_H
#define	CVT_ESSENTIALSAC_H

#include <cvt/math/sac/SampleConsensusModel.h>
#include <cvt/math/Matrix.h>
#include <cvt/vision/FeatureMatch.h>
#include <cvt/geom/PointSet.h>

namespace cvt
{
    class EssentialSAC;

    template<>
    struct SACModelTraits<EssentialSAC>
    {
        typedef Matrix3f   ResultType;
        typedef float      DistanceType;
    };

    class EssentialSAC : public SampleConsensusModel<EssentialSAC>
	{
		public:
			typedef SACModelTraits<EssentialSAC>::ResultType       ResultType;
			typedef SACModelTraits<EssentialSAC>::DistanceType     DistanceType;


			EssentialSAC( const std::vector<FeatureMatch>& matches, const Matrix3f & K ) : _matches( matches ), _K( K )
			{
				_Kinv = _K;
				if( !_Kinv.inverseSelf() ){
					throw CVTException( "Could not invert intrinsic matrix" );
				}
			}

			size_t size() const { return _matches.size(); }

			size_t minSampleSize() const
			{
				// minimum 8 correspondences to estimate
				// the essential matrix
				return 8;
			}

			ResultType estimate( const std::vector<size_t> & sampleIndices ) const;
            ResultType refine( const ResultType& res, const std::vector<size_t> & inlierIndices ) const;

			void inliers( std::vector<size_t> & inlierIndices, const ResultType & estimate, const DistanceType maxDistance ) const;

		private:
			const std::vector<FeatureMatch>&    _matches;
			Matrix3f							_K;
			Matrix3f							_Kinv;
	};

    EssentialSAC::ResultType EssentialSAC::estimate( const std::vector<size_t> & sampleIndices ) const
    {
        PointSet2f set0, set1;
        for( size_t i = 0; i < sampleIndices.size(); i++ ){
            set0.add( _matches[ sampleIndices[ i ] ].feature0->pt );
            set1.add( _matches[ sampleIndices[ i ] ].feature1->pt );
        }

        return set0.essentialMatrix( set1, _K );
    }

    EssentialSAC::ResultType EssentialSAC::refine( const ResultType&, const std::vector<size_t> & inlierIndices ) const
    {
        return estimate( inlierIndices );
    }

    void EssentialSAC::inliers( std::vector<size_t> & inlierIndices,
                                const ResultType & estimate,
                                const DistanceType maxDistance ) const
    {
		// compute the line for each point, and check the distance of the other point to it:
		Matrix3f funda = _Kinv.transpose() * estimate * _Kinv;
        Vector3f l, tmp;
        for( size_t i = 0; i < _matches.size(); i++ ){
            // calc p' = estimate * p
			tmp[ 0 ] = _matches[ i ].feature0->pt.x;
			tmp[ 1 ] = _matches[ i ].feature0->pt.y;
			tmp[ 2 ] = 1.0f;
            
			// get the line in the other image
			Line2Df line( funda *  tmp );

            if(  line.distance( _matches[ i ].feature1->pt ) < maxDistance )
                inlierIndices.push_back( i );
        }
    }
}

#endif

