#ifndef CVT_LEVENBERG_MARQUARD_H
#define CVT_LEVENBERG_MARQUARD_H

#include <Eigen/Core>
#include <Eigen/Cholesky>

#include <cvt/math/Optimization.h>
#include <cvt/math/CostFunction.h>
#include <cvt/vision/MeasurementModel.h>

namespace cvt {
	
	template<typename T>
	class LevenbergMarquard
	{
		public:
			LevenbergMarquard( const TerminationCriteria<T> & termCrit ) : 
				_termCrit( termCrit )
			{}
		
			~LevenbergMarquard(){}

			template < template <typename Type> class Derived, class AType, class bType, class ParamType, class MeasType >
			void	optimize( MeasurementModel<T, Derived, AType, bType, ParamType, MeasType> & model, const CostFunction<T, MeasType> & costFunc, TerminationType termType );			
			T		costs() const { return _lastCosts; }
			size_t	iterations() const { return _iterations; }
							
		private:
			size_t						_iterations;
			T							_lastCosts;
			TerminationCriteria<T>		_termCrit;						
	};
	
	template<typename T>
	template < template <typename Type> class Derived, class AType, class bType, class ParamType, class MeasType >
	inline void LevenbergMarquard<T>::optimize( MeasurementModel<T, Derived, AType, bType, ParamType, MeasType> & model, const CostFunction<T, MeasType> & costFunc, TerminationType termType )
	{
		T currentCosts = model.evaluateCosts( costFunc );
		
		_lastCosts = currentCosts;
		_iterations = 0;
		
		AType A;
		bType b;
		ParamType delta;
		AType lamda = AType::Identity();
		
		bool reEvaluate = true;
		
		while( !_termCrit.finished( termType, _lastCosts, _iterations ) ){
			if( reEvaluate ){
				currentCosts = model.buildLSSystem( A, b, costFunc );
			}
			
			// solve the system
			delta.setZero();
			( A + lamda ).llt().solve( b, &delta );
			
			// apply delta parameters:			
			model.apply( delta );
			
			// calculate residuals & current costs
			currentCosts = model.evaluateCosts( costFunc );
			
			if( currentCosts < _lastCosts ){
				lamda *= 0.1;
				_lastCosts = currentCosts;
				reEvaluate = true;
			} else {
				lamda *= 10.0;
				model.apply( -delta );
				reEvaluate = false;
			}
			_iterations++;
		}		
	}	
}

#endif