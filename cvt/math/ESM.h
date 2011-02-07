/*
 *  ESM.h
 *
 *  Created by Sebastian Klose on 10.10.10.
 *  Copyright 2010. All rights reserved.
 *
 */
#ifndef CVT_ESM_H
#define CVT_ESM_H

#include <cvt/gfx/Image.h>
#include <cvt/math/SL3Transform.h>

namespace cvt {
	class ESM
	{
		public:
			ESM( const Image & img, size_t maxIter = 10, double ssdEpsilon = 1e-8 );		
			virtual ~ESM();
		
			void optimize( SL3Transform & H, const Image & currI );
		
		private:	
			/* stopping citerions */
			size_t maxIter;
			double ssdEpsilon;
		
			/* the template & warped */
			Image temp;
			Image warped;
		
			/* derivatives wrt the pose parameters */
			Eigen::MatrixXd jPose;
			/* combined jacobians: (grad(temp) + grad(warped))*jPose */
			Eigen::MatrixXd jCombined;
		
			/* intensity difference: temp-warped */
			Eigen::VectorXd deltaI;
			double ssd;
			size_t iteration;
			
			/* gradients of template & current warped */
			Image tmpDx;
			Image tmpDy;
			Image warpedDx;
			Image warpedDy;				
		
			/* calc jacobians of current warped and deltaI */
			void updateData( SL3Transform & H );
		
			void calcTemplateGradients();
		
	};
}

#endif

