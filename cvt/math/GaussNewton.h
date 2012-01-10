#ifndef CVT_GAUSS_NEWTON_H
#define CVT_GAUSS_NEWTON_H

#include <Eigen/Core>
#include <Eigen/Cholesky>
#include <cvt/math/TerminationCriteria.h>
#include <cvt/math/CostFunction.h>
#include <cvt/vision/MeasurementModel.h>

namespace cvt {
	
	template<typename T>
	class GaussNewton
	{
	public:
		GaussNewton(){}		
		~GaussNewton(){}
		
		template < template <typename Type> class Derived, class AType, class bType, class ParamType, class MeasType >
		void	optimize( MeasurementModel<T, Derived, AType, bType, ParamType, MeasType> & model, 
						  const CostFunction<T, MeasType> & costFunc,
						  const TerminationCriteria<T> & termCrit );
		
		T		costs() const { return _lastCosts; }
		
		size_t	iterations() const { return _iterations; }
		
	private:
		size_t						_iterations;
		T							_lastCosts;						
	};
	
	template<typename T>
	template < template <typename Type> class Derived, class AType, class bType, class ParamType, class MeasType >
	inline void GaussNewton<T>::optimize( MeasurementModel<T, Derived, AType, bType, ParamType, MeasType> & model, 
										  const CostFunction<T, MeasType> & costFunc, 
										  const TerminationCriteria<T> & termCrit )
	{		
		_lastCosts = 0;
		_iterations = 0;
		
		AType A;
		bType b;
		ParamType delta;
						
		do {
			_lastCosts = model.buildLSSystem( A, b, costFunc );
			
			if( termCrit.finished( _lastCosts, _iterations ) )
				break;
			
			// solve the system
			delta = ParamType::Zero( A.cols() );
			delta = A.llt().solve( b );
			
			// apply delta parameters:			
			model.apply( delta );	
						
			_iterations++;
		} while( !termCrit.finished( _lastCosts, _iterations ) );		
	}	
}

#endif
