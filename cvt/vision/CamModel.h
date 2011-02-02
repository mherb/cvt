/*
 *  CamModel.h
 *  CVTools
 *
 *  Created by Sebastian Klose on 25.01.11.
 *  Copyright 2011åå. All rights reserved.
 *
 */

#ifndef CVT_CAMMODEL_H
#define CVT_CAMMODEL_H

#include <Eigen/Core>
#include <vector>

namespace cvt
{
	template <typename T>
	class CamModel
	{
		public:	
			// DAMNIT -> if we use an Eigen Type with fixed size and alignable inside classes or structs, 
			// WE HAVE TO use this, to make the operator new working correctly
			EIGEN_MAKE_ALIGNED_OPERATOR_NEW
			typedef Eigen::Matrix<T, 3, 3> MAT3;
			typedef Eigen::Matrix<T, 4, 4> MAT4;
		
			CamModel( const MAT3 & intr, const MAT4 & extr ) :
				_intrinsics( intr ),
				_extrinsics( extr )
			{
				Eigen::Matrix<T, 4, 4> K4 = Eigen::Matrix<T, 4, 4>::Zero();
				K4.block( 0, 0, 3, 3 ) = intr;
				K4( 3, 3 ) = 1;
				
				_projection = K4 * extr;
			}
		
			const MAT3 & K() const { return _intrinsics; }
			const MAT4 & camToWorld() const { return _extrinsics; }
			const MAT4 & projection() const { return _projection; }
		
		private:
			MAT3 _intrinsics;
			// from camera to world coordinate
			MAT4 _extrinsics;
			MAT4 _projection;
	};
	
	template<typename T>
	class CameraRegistry 
	{
		public:
		static CameraRegistry<T> & instance()
		{
			static CameraRegistry<T> _registry;
			return _registry;
		}
		
		size_t registerCamera( const CamModel<T> & m )
		{
			_registered.push_back( m );
			return _registered.size() - 1;
		}
		
		const CamModel<T> & model( size_t index ){ return _registered[ index ]; }
		
		private:
			CameraRegistry()
			{}
		
			~CameraRegistry()
			{}
			CameraRegistry( const CameraRegistry & other);
		
			std::vector<CamModel<T> > _registered;
	};
}

#endif