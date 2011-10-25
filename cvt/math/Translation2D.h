/*
 *  Translation2D.h
 *
 *	Translation in 2D 
 *
 *  Created by Sebastian Klose 
 *  Copyright 2011. All rights reserved.
 *
 */

#ifndef CVT_TRANSLATION2D_H
#define CVT_TRANSLATION2D_H

#include <Eigen/Core>
#include <cvt/math/Math.h>
#include <cvt/math/Matrix.h>

namespace cvt {
	/**
	 *	\class Translation2D	Translational motion in 2D
	 *	parameters:
	 *	p0:	tx
	 *	p1:	ty
	 */
	template <typename T>
	class Translation2D
	{
		public:
			static const size_t NPARAMS	= 2;
			typedef Eigen::Matrix<T, 3, 3> MatrixType;
			typedef Eigen::Matrix<T, 3, NPARAMS> JacMatType;
			typedef Eigen::Matrix<T, 3*NPARAMS, NPARAMS> HessMatType;
			typedef Eigen::Matrix<T, 2, NPARAMS> ScreenJacType;
			typedef Eigen::Matrix<T, NPARAMS, NPARAMS> ScreenHessType;
			typedef Eigen::Matrix<T, NPARAMS, 1> ParameterVectorType;
			typedef Eigen::Matrix<T, 3, 1> PointType;

			Translation2D();
			~Translation2D(){};

			void set( T tx, T ty );
			void set( const Matrix3<T> & mat );
			
			/**
			 *	\brief apply delta parameters 
			 *	\param	delta	the delta to apply
			 *	\desc	forward update of pose: T( delta ) * T( current )
			 */
			void apply( const ParameterVectorType & delta );

			/**
			 *	\brief	apply delta parameters in inverse compositional way 
			 *	\param	delta	the delta to apply
			 *	\desc	This is meant for usage in inverse compositional approaches,
			 *			where the current pose represents the transformation from the
			 *			model to the object, but the delta parameters have been estimated
			 *			in the model coords:
			 *			T(delta) * p_model = inv(T(current)) * p_current
			 *			To update T(current), we need the following:
			 *			p_model = inv( T( delta ) ) * inv(T( current ))
			 *			so the update becomes T(current) = T(current) * T(delta)
			 */
			void applyInverse( const ParameterVectorType & delta );

			/* transform the point */
			void transform( PointType & warped, const PointType & p ) const;

			/* get the jacobian at a certain point */
			void jacobian( JacMatType & J, const PointType & p ) const;

			/* hessian of the pose */
			void hessian( HessMatType & H, const PointType & p ) const;

			/* sp = proj( transform( p ) ) */
			void project( Eigen::Matrix<T, 2, 1> & sp, const PointType & p ) const;

			/* sp = proj( transform( p ) ), J = d proj( transform( p ) ) / d params */
			void project( Eigen::Matrix<T, 2, 1> & sp, ScreenJacType & J, const PointType & p ) const;

			/* p is already transformed with the current T in this case, but not yet projected! */
			void jacobianAroundT( JacMatType & J, const PointType & p ) const;

			/* sp is a screen point (transformed with current) */
			void screenJacobian( ScreenJacType & J, const Eigen::Matrix<T, 2, 1> & sp ) const;

			void screenHessian( ScreenHessType & wx, 
								ScreenHessType & wy,
							    const Eigen::Matrix<T, 2, 1> & sp ) const;

			/* get back the currently stored transformation matrix */
			const MatrixType & transformation() const { return _current; }
			MatrixType & transformation() { return _current; }

		private:
			MatrixType		_current;
	};

	template < typename T >
	inline Translation2D<T>::Translation2D() : _current( MatrixType::Identity() )
	{
	}

	template < typename T>
	inline void Translation2D<T>::set( T tx, T ty )
	{
		_current( 0, 0 ) = 1; 
		_current( 0, 1 ) = 0;
		_current( 0, 2 ) = tx;
		_current( 1, 0 ) = 0; 
		_current( 1, 1 ) = 1;
	   	_current( 1, 2 ) = ty;
		_current( 2, 0 ) = 0; 
		_current( 2, 1 ) = 0; 
		_current( 2, 2 ) = 1;
	}

	template <typename T>
	inline void Translation2D<T>::set( const Matrix3<T> & mat )
	{
		_current( 0, 0 ) = mat[ 0 ][ 0 ];
		_current( 0, 1 ) = mat[ 0 ][ 1 ];
		_current( 0, 2 ) = mat[ 0 ][ 2 ];
		_current( 1, 0 ) = mat[ 1 ][ 0 ];
		_current( 1, 1 ) = mat[ 1 ][ 1 ];
		_current( 1, 2 ) = mat[ 1 ][ 2 ];
		_current( 2, 0 ) = mat[ 2 ][ 0 ];
		_current( 2, 1 ) = mat[ 2 ][ 1 ];
		_current( 2, 2 ) = mat[ 2 ][ 2 ];
	}

	template < typename T >
	inline void Translation2D<T>::apply( const ParameterVectorType & delta )
	{
		MatrixType m;

		m( 0, 0 ) =  0;
		m( 0, 1 ) =  0;
		m( 0, 2 ) =  delta[ 0 ];

		m( 1, 0 ) =  0;
		m( 1, 1 ) =  0;
		m( 1, 2 ) =  delta[ 1 ];

		m( 2, 0 ) = 0;
		m( 2, 1 ) = 0;
		m( 2, 2 ) = 0;

		/* m = exp( m ) */
		cvt::Math::exponential( m, m );

		/* update the current transformation */
		_current = m * _current;
		//_current *= m;
	}

	template <typename T>
	inline void Translation2D<T>::applyInverse( const ParameterVectorType & delta )
	{
		MatrixType m;
		
		m( 0, 0 ) =  0;
		m( 0, 1 ) =  0;
		m( 0, 2 ) =  delta[ 0 ];

		m( 1, 0 ) =  0;
		m( 1, 1 ) =  0;
		m( 1, 2 ) =  delta[ 1 ];

		m( 2, 0 ) = 0;
		m( 2, 1 ) = 0;
		m( 2, 2 ) = 0;

		/* m = exp( m ) */
		cvt::Math::exponential( m, m );

		/* update current in the inverse fashion */
		_current *= m;
	}

	template < typename T >
	inline void Translation2D<T>::transform( PointType & warped, const PointType & p ) const
	{
		warped = _current * p;
	}

	template < typename T >
	inline void Translation2D<T>::jacobianAroundT( JacMatType & J, const PointType & p ) const
	{
		J( 0, 0 ) = p[ 2 ];
		J( 1, 0 ) =    0  ;
		J( 2, 0 ) =    0  ;

		J( 0, 1 ) =	   0  ;
		J( 1, 1 ) = p[ 2 ];
		J( 2, 1 ) =    0  ;
	}

	template < typename T >
	inline void Translation2D<T>::jacobian( JacMatType & J, const PointType & p ) const
	{
		PointType pp = _current * p;
		jacobianAroundT( J, pp );
	}

	template < typename T >
	inline void Translation2D<T>::project( Eigen::Matrix<T, 2, 1> & sp, const PointType & p ) const
	{
		PointType pp = _current * p;

		sp[ 0 ] = pp[ 0 ] / pp[ 2 ];
		sp[ 1 ] = pp[ 1 ] / pp[ 2 ];
	}

	template < typename T >
	inline void Translation2D<T>::project( Eigen::Matrix<T, 2, 1> & sp, ScreenJacType & J, const PointType & p ) const
	{
		project( sp, p );
		screenJacobian( J, sp );
	}

	template < typename T >
	inline void Translation2D<T>::screenJacobian( ScreenJacType & J, const Eigen::Matrix<T, 2, 1> & ) const
	{
		J( 0, 0 ) = 1; 
		J( 0, 1 ) = 0; 

		J( 1, 0 ) = 0;
		J( 1, 1 ) = 1;
	}


	template <typename T>
	inline void Translation2D<T>::hessian( HessMatType & H, const PointType & p ) const
	{
		H.setZero();
	}

	template <typename T>
	inline void Translation2D<T>::screenHessian( ScreenHessType & wx, 
									   ScreenHessType & wy,
									   const Eigen::Matrix<T, 2, 1> & ) const
	{
		wx = ScreenHessType::Zero();
		wy = ScreenHessType::Zero();
	}
}

#endif
