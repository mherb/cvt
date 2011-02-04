/*
 *  MeasurementModel.h
 *  CVTools
 *
 *  Created by Sebastian Klose on 03.02.11.
 *  Copyright 2011. All rights reserved.
 *
 */

#ifndef CVT_MEASUREMENTMODEL_H
#define CVT_MEASUREMENTMODEL_H

namespace cvt
{	
	template <typename T, class AType, class bType, class MeasType, class ParamType>
	class MeasurementModel
	{
		public:
			virtual			~MeasurementModel() {};
			/* build LeastSquares System  */
			virtual T		buildLSSystem( AType & A, bType & b, CostFunction<T, MeasType> & costFunc ) = 0;		
			virtual T		evaluateCosts( CostFunction<T, MeasType> & costFunc ) = 0;
			virtual void	apply( const ParamType & delta ) = 0;
	};	
}

#endif
