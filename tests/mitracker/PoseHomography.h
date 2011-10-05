#ifndef CVT_POSE_HOMOGRAPHY
#define CVT_POSE_HOMOGRAPHY

#include <Eigen/Core>

namespace cvt
{
	/**
	 *	current is the actual homography
	 *	Composition update is done by: Hcurrent * HDelta
	 *	
	 */
	template <typename T>
	class PoseHomography
	{
		public:
			PoseHomography();
			~PoseHomography();

			const Matrix3<T> & pose() const { return _current; }
				  Matrix3<T> & pose()		{ return _current; }
			

			void screenJacobian( Eigen::Matrix<T, 2, 8>& jac, const Vector2<T>& point ) const;

			void screenHessian( Eigen::Matrix<T, 8, 8>& hx, 
							    Eigen::Matrix<T, 8, 8>& hy,
							    const Vector2<T>& point ) const;

			/* construct the delta homography from the parameters and apply it to the current one */
			void addDelta( const Eigen::Matrix<T, 8, 1>& delta );
			void removeDelta( const Eigen::Matrix<T, 8, 1>& delta );
			void inverseCompositionalUpdate( const Eigen::Matrix<T, 8, 1>& delta );

		private:
			Matrix3<T>	_current;

			void fillDeltaMatrix( Matrix3<T> & deltaH, const Eigen::Matrix<T, 8, 1>& p ) const;

	};

	template <typename T>
	inline PoseHomography<T>::PoseHomography()
	{
		_current.setIdentity();
	}

	template <typename T>
	inline PoseHomography<T>::~PoseHomography()
	{
	}

	template <typename T>
	inline void PoseHomography<T>::screenJacobian( Eigen::Matrix<T, 2, 8>& j, 
												   const Vector2<T>& p ) const
	{
		T xx = p.x * p.x;
		T xy = p.x * p.y;
		T yy = p.y * p.y;
		j << p.x,   0, p.y,   0, 1, 0, -xx, -xy, 
			   0, p.x,   0, p.y, 0, 1, -xy, -yy;
	}
			
	template <typename T>
	inline void PoseHomography<T>::screenHessian( Eigen::Matrix<T, 8, 8>& hx, 
												  Eigen::Matrix<T, 8, 8>& hy,
												  const Vector2<T>& p ) const
	{
		T xx = p.x * p.x;
		T xxx = xx * p.x;
		T xxy = xx * p.y;
		T xy = p.x * p.y;
		T yy = p.y * p.y;
		T yyy = yy * p.y;
		T yyx = yy * p.x;

		// this is in the thesis
		/*
		hx <<	0,	0,	0,	0,	0,	0,	 xx,	 xy,
				0,	0,	0,	0,	0,	0,	  0,	  0, 
				0,	0,	0,	0,	0,	0,	 xy,	 yy, 
				0,	0,	0,	0,	0,	0,	  0,	  0, 
				0,	0,	0,	0,	0,	0,	p.x,    p.y, 
				0,	0,	0,	0,	0,	0,	  0,	  0, 
			   xx,	0, xy,	0, p.x,	0, -xxx,   -xxy,
			   xy,	0, yy,	0, p.y,	0, -xxy,   -yyx;
		
		hy <<	0,	0,	0,	0,	0,	 0,	   0,	  0,
				0,	0,	0,	0,	0,	 0,   xx,	 xy, 
				0,	0,	0,	0,	0,	 0,	   0,	  0, 
				0,	0,	0,	0,	0,	 0,   xy,	 yy, 
				0,	0,	0,	0,	0,	 0,    0,	  0, 
				0,	0,	0,	0,	0,	 0,  p.x,   p.y, 
			    0, xx,  0, xy,  0, p.x, -xxy,  -yyx,
			    0, xy,  0, yy,  0, p.y, -yyx,  -yyy;
		 */

		// this is what I calculated
		hx <<	0,	0,	0,	0,	0,	0,	-xx,	-xy,
				0,	0,	0,	0,	0,	0,	  0,	  0, 
				0,	0,	0,	0,	0,	0,	-xy,	-yy, 
				0,	0,	0,	0,	0,	0,	  0,	  0, 
				0,	0,	0,	0,	0,	0, -p.x,   -p.y, 
				0,	0,	0,	0,	0,	0,	  0,	  0, 
			  -xx,	0,-xy,	0,-p.x,	0,2*xxx,  2*xxy,
			  -xy,	0,-yy,	0,-p.y,	0,2*xxy,  2*yyx;
		
		hy <<	0,	0,	0,	0,	0,	 0,	   0,	  0,
				0,	0,	0,	0,	0,	 0,  -xx,	-xy, 
				0,	0,	0,	0,	0,	 0,	   0,	  0, 
				0,	0,	0,	0,	0,	 0,  -xy,	-yy, 
				0,	0,	0,	0,	0,	 0,    0,	  0, 
				0,	0,	0,	0,	0,	 0, -p.x,   -p.y, 
			    0,-xx,  0,-xy,  0,-p.x,2*xxy,  2*yyx,
			    0,-xy,  0,-yy,  0,-p.y,2*yyx,  2*yyy;
	}

	template <typename T>
	inline void PoseHomography<T>::addDelta( const Eigen::Matrix<T, 8, 1>& delta )
	{
		Matrix3<T> dH;
		fillDeltaMatrix( dH, delta );
		_current *= dH;// * _current;
	}

	template <typename T>
	inline void PoseHomography<T>::removeDelta( const Eigen::Matrix<T, 8, 1>& delta )
	{
		Matrix3<T> dH;
		fillDeltaMatrix( dH, delta );

		if( !dH.inverseSelf() ){
			throw CVTException( "Could not invert delta Matrix" );
		}

		_current = dH * _current;
	}


	template <typename T>
	inline void PoseHomography<T>::fillDeltaMatrix( Matrix3<T> & deltaH, const Eigen::Matrix<T, 8, 1>& p ) const
	{
		deltaH[ 0 ][ 0 ] = 1 + p[ 0 ];
		deltaH[ 0 ][ 1 ] = p[ 2 ];
		deltaH[ 0 ][ 2 ] = p[ 4 ];
		deltaH[ 1 ][ 0 ] = p[ 1 ];
		deltaH[ 1 ][ 1 ] = 1 + p[ 3 ];
		deltaH[ 1 ][ 2 ] = p[ 5 ];
		deltaH[ 2 ][ 0 ] = p[ 6 ];
		deltaH[ 2 ][ 1 ] = p[ 7 ];
		deltaH[ 2 ][ 2 ] = 1;
	}


	template <typename T>	
	inline void PoseHomography<T>::inverseCompositionalUpdate( const Eigen::Matrix<T, 8, 1>& delta )
	{
		removeDelta( delta );
	}

}

#endif
