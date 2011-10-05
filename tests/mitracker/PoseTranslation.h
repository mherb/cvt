#ifndef CVT_POSE_HOMOGRAPHY
#define CVT_POSE_HOMOGRAPHY

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
			PoseTranslation();
			~PoseTranslation();

			const Matrix3<T> & pose() const { return _current; }
				  Matrix3<T> & pose()		{ return _current; }
			

			void screenJacobian( Eigen::Matrix<T, 2, 2>& jac, const Vector2<T>& point ) const;

			void screenHessian( Eigen::Matrix<T, 2, 2>& hx, 
							    Eigen::Matrix<T, 2, 2>& hy,
							    const Vector2<T>& point ) const;

			/* construct the delta homography from the parameters and apply it to the current one */
			void addDelta( const Eigen::Matrix<T, 2, 1>& delta );

			void inverseCompositionalUpdate( const Eigen::Matrix<T, 2, 1>& delta );

			void removeDelta( const Eigen::Matrix<T, 2, 1>& delta );

		private:
			Matrix3<T>	_current;
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
	inline void PoseTranslation<T>::screenJacobian( Eigen::Matrix<T, 2, 2>& j, 
												    const Vector2<T>& ) const
	{
		j.setIdentity();
	}
			
	template <typename T>
	inline void PoseTranslation<T>::screenHessian( Eigen::Matrix<T, 2, 2>& hx, 
												  Eigen::Matrix<T, 2, 2>& hy,
												  const Vector2<T>& ) const
	{
		hx.setZero();
		hy.setZero();
	}

	template <typename T>
	inline void PoseTranslation<T>::addDelta( const Eigen::Matrix<T, 2, 1>& delta )
	{
		_current[ 0 ][ 2 ] += delta[ 0 ];
		_current[ 1 ][ 2 ] += delta[ 1 ];
	}

	template <typename T>
	inline void PoseTranslation<T>::removeDelta( const Eigen::Matrix<T, 2, 1>& delta )
	{
		_current[ 0 ][ 2 ] -= delta[ 0 ];
		_current[ 1 ][ 2 ] -= delta[ 1 ];
	}
		
	template <typename T>	
	inline void PoseTranslation<T>::inverseCompositionalUpdate( const Eigen::Matrix<T, 2, 1>& delta )
	{
		removeDelta( delta );
	}
}

#endif
