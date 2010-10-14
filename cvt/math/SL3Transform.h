/*
 *  SL3Transform.h
 *  CVTools
 *
 *  Created by Sebastian Klose on 08.10.10.
 *  Copyright 2010. All rights reserved.
 *
 */

#ifndef CVT_SL3TRANSFORM_H
#define CVT_SL3TRANSFORM_H

#include <Eigen/Core>
#include <vector>

namespace cvt {
	class SL3Transform
	{
		public:
			SL3Transform();
			virtual ~SL3Transform();
		
			const Eigen::Matrix3d & matrix() const
			{
				return transform;
			}
		
			/* apply delta parameters */
			void update( const Eigen::Matrix<double, 8, 1> & delta );
		
			/* get back the jacobians w.r.t. the pose parameters */
			const std::vector<Eigen::Matrix3d> & jacobians() const;
		
			/* get jacobians at a certain point */
			void jacobiansAtPoint( const Eigen::Vector3d & p, Eigen::Matrix<double, 2, 8> & J ) const;
			void jacobiansAtPoint( const Eigen::Vector2d & p, Eigen::Matrix<double, 2, 8> & J ) const;		

			/* evaluated at e -> always the same */
			static void jacobiansAtIdentity( const Eigen::Vector2d & p, Eigen::MatrixXd & J, size_t rowOffset=0 );
								
		private:
			Eigen::Matrix3d transform;	
			std::vector<Eigen::Matrix3d> jacs;
		
			void updateJacobians();
	};	
}

#endif

