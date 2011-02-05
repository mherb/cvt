#ifndef CVT_COSTFUNCTION_H
#define CVT_COSTFUNCTION_H

namespace cvt {
	
	template <typename T, class ComputeType>
	class CostFunction
	{
		public:
			CostFunction( bool robust ) : isRobust( robust ) {}
			virtual		~CostFunction(){}
			virtual		T cost( const ComputeType & r, T  & weight ) const = 0;
			const bool isRobust;
	};
	
	template<typename T, class ComputeType>
	class SquaredDistance : public CostFunction<T, ComputeType>
	{
		public:			
			
			SquaredDistance() : CostFunction<T, ComputeType>( false ) {}
			~SquaredDistance(){}
			inline T cost( const ComputeType & r, T& ) const { return 0.5 * r.squaredNorm(); }
	};
	
	template<typename T, class ComputeType>
	class RobustHuber : public CostFunction<T, ComputeType>
	{
		public:		
			RobustHuber( T k ) : CostFunction<T, ComputeType>( true ), _k( k ) {}
			~RobustHuber(){}
		
			inline T cost( const ComputeType & r, T & weight ) const 
			{
				T norm = r.norm();
				
				if( norm <= _k ){
					weight = 1; 
					return norm * norm;
				} else {
					weight = _k / norm;
					return _k * ( norm - 0.5 * _k );
				}				
			}
		
		private:
			T _k;
	};
	
}

#endif