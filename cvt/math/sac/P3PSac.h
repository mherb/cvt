/*
			CVT - Computer Vision Tools Library

 	 Copyright (c) 2012, Philipp Heise, Sebastian Klose

 	THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 	KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 	IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 	PARTICULAR PURPOSE.
 */
/*
 * Author: Sebastian Klose
 *
 * Created on July 20, 2011, 10:05 AM
 */

#ifndef CVT_P3PSAC_H
#define	CVT_P3PSAC_H

#include <cvt/math/sac/SampleConsensusModel.h>
#include <cvt/math/Matrix.h>
#include <cvt/vision/Vision.h>
#include <cvt/vision/PointCorrespondences3d2d.h>
#include <cvt/math/LevenbergMarquard.h>
#include <cvt/util/EigenBridge.h>

namespace cvt
{
    class P3PSac;

    template<>
    struct SACModelTraits<P3PSac>
    {
        typedef Matrix4d   ResultType;
        typedef double     DistanceType;
    };

    class P3PSac : public SampleConsensusModel<P3PSac>
{
      public:
        typedef SACModelTraits<P3PSac>::ResultType       ResultType;
        typedef SACModelTraits<P3PSac>::DistanceType     DistanceType;


        P3PSac( const PointSet3d & p3d, const PointSet2d & p2d, const Matrix3d & K, const Matrix3d& Kinv ); 

        size_t size() const { return _points3d.size(); }

        size_t minSampleSize() const
        {
            return 3;
        }

        ResultType estimate( const std::vector<size_t> & sampleIndices ) const;
        ResultType estimateWithInliers( const ResultType& prev, const std::vector<size_t> & sampleIndices ) const;

        ResultType refine( const ResultType& res, const std::vector<size_t> & inlierIndices ) const;

        void inliers( std::vector<size_t> & inlierIndices, const ResultType & estimate, const DistanceType maxDistance ) const;

      private:
        const PointSet3d &    _points3d;
        const PointSet2d &    _points2d;
        const Matrix3d	 &    _intrinsics;
        const Matrix3d	 &    _intrinsicsInv;
    };

        
	inline P3PSac::P3PSac( const PointSet3d & p3d, const PointSet2d & p2d, const Matrix3d & K, const Matrix3d& Kinv ) :
		_points3d( p3d ),
		_points2d( p2d ),
		_intrinsics( K ),
		_intrinsicsInv( Kinv )
    {
    }

    inline P3PSac::ResultType P3PSac::estimate( const std::vector<size_t> & sampleIndices ) const
    {
        PointSet3d worldPts;
        PointSet3d featureVecs;

		Vector3d tmp;
        Vector3d tmp2;
        for( size_t i = 0; i < sampleIndices.size(); i++ ){
            worldPts.add( _points3d[ sampleIndices[ i ] ] );
            tmp2.x = _points2d[ sampleIndices[ i ] ].x;
            tmp2.y = _points2d[ sampleIndices[ i ] ].y;
            tmp2.z = 1;
            tmp = _intrinsicsInv * tmp2;
			tmp.normalize();
            featureVecs.add( tmp );
        }
	
		std::vector<Matrix4d> results;
        Vision::p3p( results, ( Vector3d* )featureVecs.ptr(), ( Vector3d* )worldPts.ptr() );

        Vector3d pcam;
        Vector2d pp;

        double best = 10000;
        int bestIdx = -1;

        for( size_t i = 0; i < results.size(); i++ ){
            pcam = results[ i ] * worldPts[ 0 ];
            pp = _intrinsics * pcam;
            pp -= _points2d[ sampleIndices[ 0 ] ];
            double error = pp.lengthSqr();

            if( error < best ){
                best = error;
                bestIdx = i;
            }
        }

        if( bestIdx == -1 ){
            Matrix4d trans;
            trans.setIdentity();
            return trans;
        }

        return results[ bestIdx ];
    }

    inline P3PSac::ResultType P3PSac::refine( const ResultType& res, const std::vector<size_t> & inlierIndices ) const
    {
        Eigen::Matrix3d K;
        Eigen::Matrix4d ext( Eigen::Matrix4d::Identity() ), me;

        PointCorrespondences3d2d<double> pointCorresp( K, ext );
        EigenBridge::toEigen( me, res );
        pointCorresp.setPose( me );

        Eigen::Vector3d p3;
        Eigen::Vector2d p2;
        for( size_t i = 0; i < inlierIndices.size(); i++ ){
            size_t idx = inlierIndices[ i ];
           p3[ 0 ] = _points3d[ idx ].x;
           p3[ 1 ] = _points3d[ idx ].y;
           p3[ 2 ] = _points3d[ idx ].z;

           p2[ 0 ] = _points2d[ idx ].x;
           p2[ 1 ] = _points2d[ idx ].y;
           pointCorresp.add( p3, p2 );
        }

        RobustHuber<double, PointCorrespondences3d2d<double>::MeasType> costFunction( 1.0 );
        LevenbergMarquard<double> lm;
        TerminationCriteria<double> termCriteria( TERM_COSTS_THRESH | TERM_MAX_ITER );
        termCriteria.setCostThreshold( 0.1 );
        termCriteria.setMaxIterations( 20 );
        lm.optimize( pointCorresp, costFunction, termCriteria );

        me = pointCorresp.pose().transformation();

        Matrix4d refined;
        EigenBridge::toCVT( refined, me );
        return refined;
    }

    inline void P3PSac::inliers( std::vector<size_t> & inlierIndices,
                                 const ResultType & estimate,
                                 const DistanceType maxDistance ) const
    {
        // reproject all matches and compute the inliers
        Vector2d p2;
        Vector3d p3;

		/* invert the pose */
		Matrix3d R = _intrinsics * estimate.toMatrix3();
        Vector3d t( estimate[ 0 ][ 3 ], estimate[ 1 ][ 3 ], estimate[ 2 ][ 3 ] );
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

