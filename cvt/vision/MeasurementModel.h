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
	/**
	 MeasurmentModel Interface
	 CRTP is needed, because the interface is dependant on template variables 
	 (AType, bType often have known sizes at compile time) so the functions cannot be virtual
	 */
	
	template < typename T, template <typename Type> class Derived, class AType, class bType, class ParamType, class MeasType >
	class MeasurementModel
	{
		public:			
			virtual ~MeasurementModel() {};
            
			inline T buildLSSystem( AType & A, bType & b, const CostFunction<T, MeasType> & costFunc )
			{ 
				return static_cast< Derived<T>* >( this )->buildLSSystem( A, b, costFunc ); 
			}
					
			inline T evaluateCosts( const CostFunction<T, MeasType> & costFunc ) 
			{
				return static_cast< Derived<T>* >( this )->evaluateCosts( costFunc ); 
			}
		
			inline void	apply( const ParamType & delta )
			{
				static_cast< Derived<T>* >( this )->apply( delta );
			}	
	};	
}

#endif