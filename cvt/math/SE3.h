/* 
 * Special Euclidean Group Transformation:
 * 		- 3D Euclidean Transformation Parametrization using Lie Algebras
 *		- alpha, beta, gamma (angles in radians), tx, ty, tz
 */
#ifndef CVT_SE3_H
#define CVT_SE3_H

#include <cvt/math/Math.h>

#include <Eigen/Core>
#include <Eigen/Dense>
#include <Eigen/Geometry>

#include <vector>

#include <cvt/vision/CamModel.h>

namespace cvt
{
	template<typename T>
	class SE3 
	{
		typedef Eigen::Matrix<T, 4, 4> MatrixType;
		typedef Eigen::Matrix<T, 3, 6> JacMatType;
		typedef Eigen::Matrix<T, 6, 1> ParameterVectorType;
		typedef Eigen::Matrix<T, 3, 1> PointType;
		typedef Eigen::Matrix<T, 4, 1> PointTypeHom;
		
		public:
			EIGEN_MAKE_ALIGNED_OPERATOR_NEW
			/** 
			 *	angles in radians 
			 */
			SE3( T alpha = 0, T beta = 0, T gamma = 0, T tx = 0, T ty = 0, T tz = 0 );
			SE3( const ParameterVectorType & p );
			SE3( const SE3<T> & other );
			~SE3();			
			
			void set( T alpha, T beta, T gamma, T tx, T ty, T tz );
			void set( const ParameterVectorType & p );
			void set( const MatrixType & t ) { _current = t; }
			void apply( const ParameterVectorType & delta );
			void transform( PointType & warped, const PointType & p ) const;
			void jacobian( JacMatType & J, const PointType & p ) const;
			void jacobian( JacMatType & J, const PointTypeHom & p ) const;
		
			void project( Eigen::Matrix<T, 2, 1> & sp, 
						  const CamModel<T> & cam, 
						  const PointType & p3d ) const;
		
			void project( Eigen::Matrix<T, 2, 1> & sp,
						  Eigen::Matrix<T, 2, 6> & screenJac,
						  const CamModel<T> & cam,
						  const PointType & p3d ) const;
			
			/* p has to be pretransformed with the current T in this case! */
			void jacobianAroundT( JacMatType & J, const PointTypeHom & p ) const;
			
			
			const MatrixType & transformation() const 
			{ 
				return _current; 
			}
			
		private:
			MatrixType _current;

	};
	
	template <typename T>
	inline SE3<T>::SE3( T alpha, T beta, T gamma, T tx, T ty, T tz ) 
	{	
		this->set( alpha, beta, gamma, tx, ty, tz );
	}
	
	template <typename T>
	inline SE3<T>::SE3( const ParameterVectorType & p )
	{
		this->set( p[ 0 ], p[ 1 ], p[ 2 ], p[ 3 ], p[ 4 ], p[ 5 ] );
	}
	
	template <typename T>
	inline SE3<T>::SE3( const SE3<T> & other ) : _current( other._current )
	{
	}
	
	template <typename T>
	inline SE3<T>::~SE3()
	{		
	}
	
	template <typename T>
	void inline SE3<T>::set( T alpha, T beta, T gamma, T tx, T ty, T tz )
	{
		_current.block( 0, 0, 3, 3 ) = ( Eigen::AngleAxis<T>( alpha, Eigen::Matrix<T, 3, 1>::UnitX() ) * 
										 Eigen::AngleAxis<T>( beta, Eigen::Matrix<T, 3, 1>::UnitY() ) *
										 Eigen::AngleAxis<T>( gamma, Eigen::Matrix<T, 3, 1>::UnitZ() ) ).toRotationMatrix();
		_current( 0, 3 ) = tx; 
		_current( 1, 3 ) = ty;
		_current( 2, 3 ) = tz;
		_current( 3, 0 ) = 0; 
		_current( 3, 1 ) = 0; 
		_current( 3, 2 ) = 0; 
		_current( 3, 3 ) = 1.0;
	}
	
	template<typename T>
	void inline SE3<T>::set( const ParameterVectorType & p )
	{
		this->set( p[ 0 ], p[ 1 ], p[ 2 ], p[ 3 ], p[ 4 ], p[ 5 ] );
	}
	
	template <typename T>
	inline void SE3<T>::apply( const ParameterVectorType & delta )
	{
		// exponential can be evaluated in closed form in this case:
		T theta = Math::sqrt( Math::sqr( delta[ 0 ] ) + Math::sqr( delta[ 1 ] ) + Math::sqr( delta[ 2 ] ) );
		
		Eigen::Matrix<T, 4, 4> trans;
		
		if( theta == 0 ){
			trans( 0, 0 ) = 1.0; trans( 0, 1 ) = 0.0; trans( 0, 2 ) = 0.0; trans( 0, 3 ) = delta[ 3 ];
			trans( 1, 0 ) = 0.0; trans( 1, 1 ) = 1.0; trans( 1, 2 ) = 0.0; trans( 1, 3 ) = delta[ 4 ];
			trans( 2, 0 ) = 0.0; trans( 2, 1 ) = 0.0; trans( 2, 2 ) = 1.0; trans( 2, 3 ) = delta[ 5 ];
			trans( 3, 0 ) = 0.0; trans( 3, 1 ) = 0.0; trans( 3, 2 ) = 0.0; trans( 3, 3 ) =        1.0;
		} else {			
			T a = Math::sin( theta ) / theta;
			T thetaSqr = theta * theta;
			T b = ( 1 - Math::cos( theta ) ) / ( thetaSqr );			
			
			Eigen::Matrix<T, 3, 3> angleSkew;
			angleSkew( 0, 0 ) =         0  ; angleSkew( 0, 1 ) = -delta[ 2 ]; angleSkew( 0, 2 ) =  delta[ 1 ];
			angleSkew( 1, 0 ) =  delta[ 2 ]; angleSkew( 1, 1 ) =         0  ; angleSkew( 1, 2 ) = -delta[ 0 ];
			angleSkew( 2, 0 ) = -delta[ 1 ]; angleSkew( 2, 1 ) =  delta[ 0 ]; angleSkew( 2, 2 ) =         0  ;
			
			Eigen::Matrix<T, 3, 3> angleSkewSqr = angleSkew * angleSkew;
			
			trans.template block<3, 3>( 0, 0 ) = Eigen::Matrix<T, 3, 3>::Identity() + a * angleSkew + b * angleSkewSqr;					
			trans.template block<3, 1>( 0, 3 ) = ( Eigen::Matrix<T, 3, 3>::Identity() + b * angleSkew + (1 - a ) / thetaSqr * angleSkewSqr ) * delta.template segment<3>( 3 );
			
			trans( 3, 0 ) = 0; trans( 3, 1 ) = 0; trans( 3, 2 ) = 0; trans( 3, 3 ) = 1;
		}
				
		_current = trans * _current;
	}
	
	template <typename T>
	inline void SE3<T>::jacobian( JacMatType & J, const PointType & pp ) const
	{
		Eigen::Matrix<T, 4, 1> p;	
		p.template segment<3>( 0 ) = pp;
		p[ 3 ] = 1.0;
		
		this->jacobian( J, p );
	}
	
	template <typename T>
	inline void SE3<T>::jacobian( JacMatType & J, const PointTypeHom & pp ) const
	{
		PointTypeHom p = _current * pp;	
		jacobianAroundT( J, p );
	}
	
	template <typename T>
	inline void SE3<T>::jacobianAroundT( JacMatType & J, const PointTypeHom & p ) const
	{		
		J( 0, 0 ) =	    0  ; J( 0, 1 ) =  p[ 2 ]; J( 0, 2 ) = -p[ 1 ]; J( 0, 3 ) = p[ 3 ]; J( 0, 4 ) =    0  ; J( 0, 5 ) = 0;
		J( 1, 0 ) = -p[ 2 ]; J( 1, 1 ) =	 0  ; J( 1, 2 ) =  p[ 0 ]; J( 1, 3 ) =    0  ; J( 1, 4 ) = p[ 3 ]; J( 1, 5 ) = 0;
		J( 2, 0 ) =  p[ 1 ]; J( 2, 1 ) = -p[ 0 ]; J( 2, 2 ) =	  0  ; J( 2, 3 ) =    0  ; J( 2, 4 ) =    0  ; J( 2, 5 ) = p[ 3 ];
	}
	
	template<typename T>
	inline void SE3<T>::transform( PointType & warped, const PointType & p ) const
	{		
		warped = _current.template block<3, 3>( 0, 0 ) * p + _current.template block<3, 1>( 0, 3 );
	}
	
	template < typename T >
	inline void SE3<T>::project( Eigen::Matrix<T, 2, 1> & sp, const CamModel<T> & cam, const PointType & p3d ) const
	{
		PointTypeHom pHom;
		pHom.template segment<3>( 0 ) = p3d;
		pHom[ 3 ] = 1.0;
		
		pHom = cam.projection() * _current * pHom;
		sp[ 0 ] = pHom[ 0 ] / pHom[ 2 ];
		sp[ 1 ] = pHom[ 1 ] / pHom[ 2 ];
	}
	
	template < typename T > 
	inline void SE3<T>::project( Eigen::Matrix<T, 2, 1> & sp,
								 Eigen::Matrix<T, 2, 6> & screenJac,
								 const CamModel<T> & cam,
								 const PointType & p3d ) const
	{
		PointTypeHom pHom, p;
		pHom.template segment<3>( 0 ) = p3d;
		pHom[ 3 ] = 1.0;
		
		p = _current * pHom;
		Eigen::Matrix<T, 3, 6> poseJ;
		this->jacobianAroundT( poseJ, p );
		
		pHom = cam.projection() * p;
		sp[ 0 ] = pHom[ 0 ] / pHom[ 2 ];
		sp[ 1 ] = pHom[ 1 ] / pHom[ 2 ];
		
		const Eigen::Matrix<T, 3, 3> & K = cam.K();
		Eigen::Matrix<T, 2, 3> projJ;
		projJ( 0, 0 ) = K( 0, 0 ) / pHom[ 2 ]; 
		projJ( 0, 1 ) = K( 0, 1 ) / pHom[ 2 ]; 
		projJ( 0, 2 ) = ( K( 0, 2 ) - sp[ 0 ] ) / pHom[ 2 ];
		
		projJ( 1, 0 ) = 0;
		projJ( 1, 1 ) = K( 1, 1 ) / pHom[ 2 ]; 
		projJ( 1, 2 ) = ( K( 1, 2 ) - sp[ 1 ] ) / pHom[ 2 ];
		
		screenJac = projJ * poseJ;
	}
}

#endif
