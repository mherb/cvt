#ifndef CVT_COSTFUNCTION_H
#define CVT_COSTFUNCTION_H

namespace cvt {
	template<typename T, class ComputeType>
	class CostFunction
	{
		public:
			CostFunction(){}
			~CostFunction(){}
		
			T costs( const ComputeType * residuals, size_t n );
			T cost( const ComputeType & residual );
	};
	
	template<typename T, class ComputeType>
	inline T CostFunction<T, ComputeType>::costs( const ComputeType * residuals, size_t n )
	{
		T c = 0;
		while ( n-- ) {			
			c += residuals->squaredNorm();
			residuals++;
		}
		return c;
	}
	
	template<typename T, class ComputeType>
	inline T CostFunction<T, ComputeType>::cost( const ComputeType & r )
	{
		return r.squaredNorm();
	}

}

#endif