/*
			CVT - Computer Vision Tools Library

 	 Copyright (c) 2012, Philipp Heise, Sebastian Klose

 	THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 	KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 	IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 	PARTICULAR PURPOSE.
 */
#ifndef CVT_COSTFUNCTION_H
#define CVT_COSTFUNCTION_H

#include <cvt/math/Math.h>

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
	
	template <typename T>
	class SquaredDistance<T, T> : public CostFunction<T, T>
	{
		public:
			SquaredDistance() : CostFunction<T, T>( false ) {}
			~SquaredDistance(){}
			inline T cost( const T & r, T& ) const { return r*r; }
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
	
	template< typename T >
	class RobustHuber<T, T> : public CostFunction<T, T>
	{
	public:		
		RobustHuber( T k ) : CostFunction<T, T>( true ), _k( k ) {}
		~RobustHuber(){}
		
		inline T cost( const T & r, T & weight ) const 
		{
			T norm = Math::abs( r );
			
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
	

	template< typename T >
	class L1 : public CostFunction<T, T>
	{
	public:		
		L1() : CostFunction<T, T>( true ) {}
		~L1(){}
		
		inline T cost( const T & r, T & weight ) const 
		{
			weight = ( T ) 1 / ( Math::abs( r ) + 1e-5 );
			return Math::abs( r );
		}
		
	};
}

#endif
