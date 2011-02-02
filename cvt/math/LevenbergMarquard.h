#ifndef CVT_LEVENBERG_MARQUARD_H
#define CVT_LEVENBERG_MARQUARD_H

#include <Eigen/Core>
#include <Eigen/Cholesky>

#include <cvt/vision/CamModel.h>
#include <cvt/math/Optimization.h>
#include <cvt/math/CostFunction.h>
#include <cvt/math/SE3.h>
#include <cvt/vision/PointMeasurements.h>
#include <cvt/vision/Warp.h>

namespace cvt {
	
	template<typename T>
	class LevenbergMarquard
	{
		public:
			typedef Eigen::Matrix<T, 3, 1> InVec;
			typedef Eigen::Matrix<T, 2, 1> OutVec;
			typedef Eigen::Matrix<T, 6, 1> ParamVec;
		
			LevenbergMarquard( const TerminationCriteria<T> & termCrit ) : _termCrit( termCrit )
			{}
		
			~LevenbergMarquard(){}
			
			void optimize( SE3<T> & model, const PointCorrespondences3d2d<T> & measurements, TerminationType termType );
			
			T		costs() const { return _lastCosts; }
			size_t	iterations() const { return _iterations; }
							
		private:
			size_t			_iterations;
			T				_lastCosts;
		
			CostFunction<T, OutVec>		_costFunc;
			TerminationCriteria<T>		_termCrit;						
	};
	
	template<typename T>
	inline void LevenbergMarquard<T>::optimize( SE3<T> & model, const PointCorrespondences3d2d<T> & data, TerminationType termType )
	{				
		size_t numMeas = data.size();		
		OutVec r;
		OutVec w;
						
		CameraRegistry<T> & camReg = CameraRegistry<T>::instance();		
		const CamModel<T> & camModel = camReg.model( data.cameraId() );

		typename PointCorrespondences3d2d<T>::ConstDataPtr d = data.data();
		typename PointCorrespondences3d2d<T>::ConstMeasPtr m = data.meas();
		
		T currentCosts = 0;
		
		// calculate residuals & current costs
		for( size_t i = 0; i < numMeas; i++ ){
			Warp::toScreen<T>( w, camModel, model, *d );
			r = *m - w;			
			currentCosts += _costFunc.cost( r );						
			m++; 
			d++;
		}
				
		_lastCosts = currentCosts;
		_iterations = 0;
		
		Eigen::Matrix<T, 2, 6> J;
		Eigen::Matrix<T, 6, 6> A;
		Eigen::Matrix<T, 6, 1> b, delta;
		Eigen::Matrix<T, 6, 6> lamda = Eigen::Matrix<T, 6, 6>::Identity();
		
		bool reEvaluate = true;
		
		while( !_termCrit.finished( termType, _lastCosts, _iterations ) ){
			if( reEvaluate ){
				d = data.data();
				m = data.meas();
				A.setZero();
				b.setZero();			
				for( size_t i = 0; i < numMeas; i++ ){							
					Warp::toScreenAndJacobian<T>( w, J, camModel, model, *d );
					
					r = *m - w;
					
					A += ( J.transpose() * J );
					b += ( J.transpose() * ( r ) );
					
					m++;
					d++;
				}			
			}
			
			// solve the system
			delta.setZero();
			( A + lamda ).llt().solve( b, &delta );
			
			// apply delta parameters:			
			model.apply( delta );
			
			d = data.data();
			m = data.meas();
			
			// calculate residuals & current costs
			currentCosts = 0;
			for( size_t i = 0; i < numMeas; i++ ){
				Warp::toScreen<T>( w, camModel, model, *d );
				r = *m - w;
				currentCosts += _costFunc.cost( r );
				m++;
				d++;
			}
			
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