/*
			CVT - Computer Vision Tools Library

 	 Copyright (c) 2012, Philipp Heise, Sebastian Klose

 	THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 	KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 	IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 	PARTICULAR PURPOSE.
 */
/*
 * Author: sebi
 *
 * Created on July 20, 2011, 10:05 AM
 */

#ifndef CVT_EPNPSAC_H
#define	CVT_EPNPSAC_H

#include <cvt/math/sac/SampleConsensusModel.h>
#include <cvt/math/Matrix.h>
#include <cvt/vision/EPnP.h>


namespace cvt
{
    class EPnPSAC;

    template<>
    struct SACModelTraits<EPnPSAC>
    {
        typedef Matrix4d   ResultType;
        typedef double      DistanceType;
    };

    class EPnPSAC : public SampleConsensusModel<EPnPSAC>
{
      public:
        typedef SACModelTraits<EPnPSAC>::ResultType       ResultType;
        typedef SACModelTraits<EPnPSAC>::DistanceType     DistanceType;


        EPnPSAC( const PointSet3d & p3d, const PointSet2d & p2d, const Matrix3d & K ); 

        size_t size() const { return _points3d.size(); }

        size_t minSampleSize() const
        {
            // what is a good number for EPnP?
            // In principle it should work with 4
            return 6;
        }

        ResultType estimate( const std::vector<size_t> & sampleIndices ) const;

        ResultType refine( const std::vector<size_t> & inlierIndices ) const;

        void inliers( std::vector<size_t> & inlierIndices, const ResultType & estimate, const DistanceType maxDistance ) const;

      private:
        const PointSet3d &    _points3d;
        const PointSet2d &    _points2d;
        const Matrix3d	 &    _intrinsics;
    };

        
	inline EPnPSAC::EPnPSAC( const PointSet3d & p3d, const PointSet2d & p2d, const Matrix3d & K ) :
		_points3d( p3d ),
		_points2d( p2d ),
		_intrinsics( K )
    {
    }

    inline EPnPSAC::ResultType EPnPSAC::estimate( const std::vector<size_t> & sampleIndices ) const
    {
        PointSet3d p3d;
        PointSet2d p2d;

        for( size_t i = 0; i < sampleIndices.size(); i++ ){
            p3d.add( _points3d[ sampleIndices[ i ] ] );
            p2d.add( _points2d[ sampleIndices[ i ] ] );
        }
		
		EPnPd epnp( p3d );

		Matrix4d trans;
		epnp.solve( trans, p2d, _intrinsics );

        return trans; 
    }

    inline EPnPSAC::ResultType EPnPSAC::refine( const std::vector<size_t> & inlierIndices ) const
    {
        // TODO: would be nicer, to use estimate, to get a linear estimate,
        //       and then refine it iteratively using GN or LM e.g.
        return estimate( inlierIndices );
    }

    inline void EPnPSAC::inliers( std::vector<size_t> & inlierIndices,
                                 const ResultType & estimate,
                                 const DistanceType maxDistance ) const
    {
        // reproject all matches and compute the inliers
        Vector2d p2;
        Vector3d p3;

		/* invert the pose */
		Matrix3d R = _intrinsics * estimate.toMatrix3();
		Vector3d t( estimate[ 0 ][ 3 ], estimate[ 1 ][ 3 ], estimate[ 2 ][ 2 ] );
		// apply intrinsics
		t = _intrinsics * t;

        for( size_t i = 0; i < _points3d.size(); i++ ){
            // calc p' = estimate * p
            p3 = R * _points3d[ i ] + t;

			if( Math::abs( p3.z ) < 1e-6 )
				continue;

			p2.x = p3.x / p3.z;
			p2.y = p3.y / p3.z;

            if( ( p2 - _points2d[ i ] ).length() < maxDistance )
                inlierIndices.push_back( i );
        }
    }
}

#endif

