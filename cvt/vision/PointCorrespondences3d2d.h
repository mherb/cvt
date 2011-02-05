/*
 *  PointCorrespondence3d2d.h
 *  CVTools
 *
 *  Created by Sebastian Klose on 03.02.11.
 *  Copyright 2011. All rights reserved.
 *
 */

#ifndef CVT_POINTCORRESPONDENCES_3D2D_H
#define CVT_POINTCORRESPONDENCES_3D2D_H

#include <cvt/vision/MeasurementModel.h>
#include <cvt/vision/CamModel.h>
#include <cvt/math/SE3.h>

namespace cvt {

	template <typename T>
	class PointCorrespondences3d2d : public MeasurementModel 
		< T, 
		  PointCorrespondences3d2d, 
		  Eigen::Matrix<T, 6, 6>, 
          Eigen::Matrix<T, 6, 1>, 
          Eigen::Matrix<T, 6, 1>,
          Eigen::Matrix<T, 2, 1> >
	{
		public:				
			typedef typename Eigen::Matrix<T, 6, 6> AType;
			typedef typename Eigen::Matrix<T, 6, 1> bType;
			typedef typename Eigen::Matrix<T, 6, 1> ParamType;
			typedef typename Eigen::Matrix<T, 2, 1> MeasType;
			
		
			PointCorrespondences3d2d( const CamModel<T> & cam );
			~PointCorrespondences3d2d();
		
			T		buildLSSystem( AType & A, bType & b, const CostFunction<T, MeasType> & costFunc );			
			T		evaluateCosts( const CostFunction<T, MeasType> & costFunc );		
			void	apply( const ParamType & delta );
		
			void				add( const Eigen::Matrix<T, 3, 1> & p3d, const Eigen::Matrix<T, 2, 1> & p2d );
			const SE3<T> &		pose() const { return _pose; }
			const CamModel<T> &	camModel() const { return _cam; }
		
		private:
			SE3<T>		_pose;
			CamModel<T>	_cam;
			std::vector<Eigen::Matrix<T, 3, 1> >	_data;
			std::vector<Eigen::Matrix<T, 2, 1> >	_meas;
	};
	
	template < typename T >  
	inline PointCorrespondences3d2d<T>::PointCorrespondences3d2d( const CamModel<T> & cam ) : _cam( cam ) {}
	
	template < typename T > inline PointCorrespondences3d2d<T>::~PointCorrespondences3d2d() {}

	template < typename T >
	inline T PointCorrespondences3d2d<T>::buildLSSystem( Eigen::Matrix<T, 6, 6> & A, 
														 Eigen::Matrix<T, 6, 1> & b,
														 const CostFunction<T, MeasType> & costFunc )
	{
		T costs = 0;
		
		A.setZero();
		b.setZero();
		Eigen::Matrix<T, 2, 1> w, r;
		Eigen::Matrix<T, 2, 6> J;
		Eigen::Matrix<T, 2, 2> weighting = Eigen::Matrix<T, 2, 2>::Identity();
        
        T weight;
		
		for( size_t i = 0; i < _data.size(); i++ ){							
			_pose.project( w, J, _cam, _data[ i ] );
			r = _meas[ i ] - w;		

			costs += costFunc.cost( r, weight );
			
			// TODO: if the costfunction is robust, the jacobians and residuals need to be weighted
			if( costFunc.isRobust ){
				weighting( 0, 0 ) = weighting( 1, 1 ) = weight;
				A += ( J.transpose() * weighting * J );
				b +=( J.transpose() * weighting * r );
			} else {
				A += ( J.transpose() * J );
				b +=( J.transpose() * r );
			}
		}
		return costs;
	}
	
	template < typename T >
	inline T PointCorrespondences3d2d<T>::evaluateCosts( const CostFunction<T, MeasType> & costFunc )
	{
		T costs = 0;
		T weight;
		Eigen::Matrix<T, 2, 1> p;

		for( size_t i = 0; i < _data.size(); i++ ){
			_pose.project( p, _cam, _data[ i ] );
			costs += costFunc.cost( _meas[ i ] - p, weight );
		}
		
		return costs;
	}
	
	template < typename T >
	inline void	PointCorrespondences3d2d<T>::apply( const Eigen::Matrix<T, 6, 1> & delta )
	{
		_pose.apply( delta );
	}
	
	template < typename T >
	inline void	PointCorrespondences3d2d<T>::add( const Eigen::Matrix<T, 3, 1> & p3d, const Eigen::Matrix<T, 2, 1> & p2d )
	{
		_data.push_back( p3d );
		_meas.push_back( p2d );
	}
}
#endif