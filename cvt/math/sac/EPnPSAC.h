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
	template <typename T> class EPnPSAC;

    template<>
	struct SACModelTraits<EPnPSAC<float> >
    {
		typedef Matrix4f	ResultType;
		typedef float		DistanceType;
    };
	template<>
	struct SACModelTraits<EPnPSAC<double> >
	{
		typedef Matrix4d	ResultType;
		typedef double      DistanceType;
	};

	template <typename T>
	class EPnPSAC : public SampleConsensusModel<EPnPSAC<T> >
	{
      public:
		typedef typename SACModelTraits<EPnPSAC<T> >::ResultType	ResultType;
		typedef typename SACModelTraits<EPnPSAC>::DistanceType		DistanceType;


		EPnPSAC( const PointSet<3, T> & p3d, const PointSet<2, T> & p2d, const Matrix3<T> & K );

        size_t size() const { return _points3d.size(); }

        size_t minSampleSize() const
        {
            // what is a good number for EPnP?
			// In theory it should work with 4
            return 5;
        }

        ResultType estimate( const std::vector<size_t> & sampleIndices ) const;
        ResultType refine( const ResultType& res, const std::vector<size_t> & inlierIndices ) const;

        void inliers( std::vector<size_t> & inlierIndices, const ResultType & estimate, const DistanceType maxDistance ) const;

      private:
		const PointSet<3, T> &    _points3d;
		const PointSet<2, T> &    _points2d;
		const Matrix3<T>	 &    _intrinsics;
    };

        
	template <typename T>
	inline EPnPSAC<T>::EPnPSAC( const PointSet<3, T> & p3d, const PointSet<2, T> & p2d, const Matrix3<T> & K ) :
		_points3d( p3d ),
		_points2d( p2d ),
		_intrinsics( K )
    {
    }

	template <typename T>
	inline typename EPnPSAC<T>::ResultType EPnPSAC<T>::estimate( const std::vector<size_t> & sampleIndices ) const
    {
		PointSet<3, T> p3d;
		PointSet<2, T> p2d;

        for( size_t i = 0; i < sampleIndices.size(); i++ ){
            p3d.add( _points3d[ sampleIndices[ i ] ] );
            p2d.add( _points2d[ sampleIndices[ i ] ] );
        }
		
		EPnP<T> epnp( p3d );

		Matrix4<T> trans;
		epnp.solve( trans, p2d, _intrinsics );

        return trans; 
    }

	template <typename T>
	inline typename EPnPSAC<T>::ResultType EPnPSAC<T>::refine( const ResultType&, const std::vector<size_t> & inlierIndices ) const
    {
        // TODO: would be nicer, to use estimate, to get a linear estimate,
        //       and then refine it iteratively using GN or LM e.g.
        return estimate( inlierIndices );
    }

	template <typename T>
	inline void EPnPSAC<T>::inliers( std::vector<size_t> & inlierIndices,
									 const ResultType & estimate,
									 const DistanceType maxDistance ) const
    {
        // reproject all matches and compute the inliers
		Vector2<T> p2;
		Vector3<T> p3;

		/* invert the pose */
		Matrix3<T> R = _intrinsics * estimate.toMatrix3();
		Vector3<T> t( estimate[ 0 ][ 3 ], estimate[ 1 ][ 3 ], estimate[ 2 ][ 3 ] );

		// apply intrinsics
		t = _intrinsics * t;

        for( size_t i = 0; i < _points3d.size(); i++ ){
            // calc p' = estimate * p
            p3 = R * _points3d[ i ] + t;

			if( Math::abs( p3.z ) < ( T )1e-6 )
				continue;

			p2.x = p3.x / p3.z;
			p2.y = p3.y / p3.z;

            if( ( p2 - _points2d[ i ] ).length() < maxDistance )
                inlierIndices.push_back( i );
        }
    }

}

#endif

