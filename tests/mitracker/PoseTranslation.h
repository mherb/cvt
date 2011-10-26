#ifndef CVT_POSE_TRANSLATION_H
#define CVT_POSE_TRANSLATION_H

#include <Eigen/Core>

namespace cvt
{
	/**
	 *	current is the actual translation
	 *	
	 */
	template <typename T>
	class PoseTranslation
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
			
			PoseTranslation();
			~PoseTranslation();

			const MatrixType & transformation() const { return _current; }
			MatrixType & transformation() { return _current; }
			
			void set( const Matrix3<T> & mat );

			void screenJacobian( ScreenJacType& jac, const Eigen::Matrix<T, 2, 1> & point ) const;

			void screenHessian( ScreenHessType& hx, 
							    ScreenHessType& hy,
							    const Eigen::Matrix<T, 2, 1> & point ) const;

			/* construct the delta homography from the parameters and apply it to the current one */
			void apply( const ParameterVectorType& delta );
			void applyInverse( const ParameterVectorType& delta );

		private:
			Eigen::Matrix<T, 3, 3>	_current;
	};

	template <typename T>
	inline PoseTranslation<T>::PoseTranslation()
	{
		_current.setIdentity();
	}

	template <typename T>
	inline PoseTranslation<T>::~PoseTranslation()
	{
	}

	template <typename T>
	inline void PoseTranslation<T>::set( const Matrix3<T> & mat )
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

	template <typename T>
	inline void PoseTranslation<T>::screenJacobian( Eigen::Matrix<T, 2, 2>& j, 
												    const Eigen::Matrix<T, 2, 1>& ) const
	{
		j.setIdentity();
	}
			
	template <typename T>
	inline void PoseTranslation<T>::screenHessian( Eigen::Matrix<T, 2, 2>& hx, 
												   Eigen::Matrix<T, 2, 2>& hy,
												   const Eigen::Matrix<T, 2, 1>& ) const
	{
		hx.setZero();
		hy.setZero();
	}

	template <typename T>
	inline void PoseTranslation<T>::apply( const Eigen::Matrix<T, 2, 1>& delta )
	{
		_current( 0 , 2 ) += delta[ 0 ];
		_current( 1 , 2 ) += delta[ 1 ];
	}

	template <typename T>
	inline void PoseTranslation<T>::applyInverse( const Eigen::Matrix<T, 2, 1>& delta )
	{
		_current( 0 , 2 ) += delta[ 0 ];
		_current( 1 , 2 ) += delta[ 1 ];
	}
		
}

#endif
