/*
 *  Warp.h
 *  CVTools
 *
 *  Created by Sebastian Klose on 26.01.11.
 *  Copyright 2011. All rights reserved.
 *
 */

#ifndef CVT_WARP_H
#define CVT_WARP_H

#include <cvt/vision/CamModel.h>
#include <cvt/math/SE3.h>
#include <Eigen/Core>

namespace cvt
{
	class Warp
	{
		public:
			template <typename T>
			static void toScreen( Eigen::Matrix<T, 2, 1> & point2d, 
								  const CamModel<T> & cam,
								  const SE3<T> & pose,
								  const Eigen::Matrix<T, 3, 1> & point3d );
		
			template <typename T>
			static void toScreen( Eigen::Matrix<T, 2, 1> & point2d, 
								  const CamModel<T> & cam,
								  const SE3<T> & pose,
								  const Eigen::Matrix<T, 4, 1> & point3d );
		
			template <typename T>
			static void toScreenAndJacobian( Eigen::Matrix<T, 2, 1> & point2d, 
											 Eigen::Matrix<T, 2, 6> & J,
											 const CamModel<T> & cam, 
											 const SE3<T> & pose, 
											 const Eigen::Matrix<T, 3, 1> & point3d );
			
	};
	
	template <typename T>
	void Warp::toScreen( Eigen::Matrix<T, 2, 1> & point2d, 
						 const CamModel<T> & cam,
						 const SE3<T> & pose,
						 const Eigen::Matrix<T, 4, 1> & point3d )
	{
		Eigen::Matrix<T, 4, 1> p;
		p = cam.projection() * pose.transformation() * point3d;
		
		point2d[ 0 ] = p[ 0 ] / p[ 2 ];
		point2d[ 1 ] = p[ 1 ] / p[ 2 ];
	}
	
	template<typename T>
	inline void Warp::toScreen( Eigen::Matrix<T, 2, 1> & point2d, 
								const CamModel<T> & cam, 
								const SE3<T> & pose, 
								const Eigen::Matrix<T, 3, 1> & point3d )
	{
		Eigen::Matrix<T, 4, 1> pHom, p;
		pHom[ 0 ] = point3d[ 0 ];
		pHom[ 1 ] = point3d[ 1 ];
		pHom[ 2 ] = point3d[ 2 ];
		pHom[ 3 ] = 1;

		p = cam.projection() * pose.transformation() * pHom;
		
		point2d[ 0 ] = p[ 3 ] * p[ 0 ] / p[ 2 ];
		point2d[ 1 ] = p[ 3 ] * p[ 1 ] / p[ 2 ];
	}
	
	template <typename T>
	inline void Warp::toScreenAndJacobian( Eigen::Matrix<T, 2, 1> & point2d, 
										   Eigen::Matrix<T, 2, 6> & J,
										   const CamModel<T> & cam, 
										   const SE3<T> & pose, 
										   const Eigen::Matrix<T, 3, 1> & point3d )
	{
		Eigen::Matrix<T, 4, 1> pHom, pCam;
		Eigen::Matrix<T, 3, 1> screenHom, camDehom;
		pHom[ 0 ] = point3d[ 0 ];
		pHom[ 1 ] = point3d[ 1 ];
		pHom[ 2 ] = point3d[ 2 ];
		pHom[ 3 ] = 1;

		Eigen::Matrix<T, 3, 6> jT;		
		pose.jacobian( jT, point3d );
		
		pCam = cam.camToWorld() * pose.transformation() * pHom;
		camDehom = pCam.template segment<3>( 0 );
		
		T tmp = pCam[ 2 ];
				
		const Eigen::Matrix<T, 3, 3> & K = cam.K();
		screenHom = K * camDehom;
		point2d[ 0 ] = screenHom[ 0 ] / screenHom[ 2 ];
		point2d[ 1 ] = screenHom[ 1 ] / screenHom[ 2 ];
		
		Eigen::Matrix<T, 2, 3> jP;
		jP( 0, 0 ) = K( 0, 0 ) / tmp; jP( 0, 1 ) = K( 0, 1 ) / tmp; jP( 0, 2 ) = ( K( 0, 2 ) - point2d[ 0 ] ) / tmp;
		jP( 1, 0 ) = 0;				  jP( 1, 1 ) = K( 1, 1 ) / tmp; jP( 1, 2 ) = ( K( 1, 2 ) - point2d[ 1 ] ) / tmp;
		
		J = jP * jT;
	}	
}

#endif
