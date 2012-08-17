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
			static const size_t NPARAMS	= 8;
			typedef Eigen::Matrix<T, 3, 3> MatrixType;
			typedef Eigen::Matrix<T, 3, NPARAMS> JacMatType;
			typedef Eigen::Matrix<T, 3*NPARAMS, NPARAMS> HessMatType;
			typedef Eigen::Matrix<T, 2, NPARAMS> ScreenJacType;
			typedef Eigen::Matrix<T, NPARAMS, NPARAMS> ScreenHessType;
			typedef Eigen::Matrix<T, NPARAMS, 1> ParameterVectorType;
			typedef Eigen::Matrix<T, 2, 1> PointType;

			PoseHomography();
			~PoseHomography();

			void set( const Matrix3<T> & mat );

			/* get back the currently stored transformation matrix */
			const MatrixType & transformation() const { return _current; }
			MatrixType & transformation() { return _current; }

			void screenJacobian( ScreenJacType& jac, const PointType& point ) const;

			void screenHessian( ScreenHessType& hx, 
							    ScreenHessType& hy,
							    const PointType& point ) const;

			/* construct the delta homography from the parameters and apply it to the current one */
			void apply( const ParameterVectorType& delta );
			void applyInverse( const ParameterVectorType& delta );
			void inverseCompositionalUpdate( const ParameterVectorType& delta );

		private:
			MatrixType	_current;

			void fillDeltaMatrix( MatrixType & deltaH, const ParameterVectorType& p ) const;

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
	inline void PoseHomography<T>::set( const Matrix3<T> & mat )
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
	inline void PoseHomography<T>::screenJacobian( ScreenJacType& j, 
												   const PointType& p ) const
	{
		T xx = p.x() * p.x();
		T xy = p.x() * p.y();
		T yy = p.y() * p.y();
		j << p.x(),   0, p.y(),   0, 1, 0, -xx, -xy, 
			   0, p.x(),   0, p.y(), 0, 1, -xy, -yy;
	}
			
	template <typename T>
	inline void PoseHomography<T>::screenHessian( ScreenHessType& hx, 
												  ScreenHessType& hy,
												  const PointType& p ) const
	{
		T xx = p.x() * p.x();
		T xxx = xx * p.x();
		T xxy = xx * p.y();
		T xy = p.x() * p.y();
		T yy = p.y() * p.y();
		T yyy = yy * p.y();
		T yyx = yy * p.x();

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
				0,	0,	0,	0,	0,	0, -p.x(),   -p.y(), 
				0,	0,	0,	0,	0,	0,	  0,	  0, 
			  -xx,	0,-xy,	0,-p.x(),	0,2*xxx,  2*xxy,
			  -xy,	0,-yy,	0,-p.y(),	0,2*xxy,  2*yyx;
		
		hy <<	0,	0,	0,	0,	0,	 0,	   0,	  0,
				0,	0,	0,	0,	0,	 0,  -xx,	-xy, 
				0,	0,	0,	0,	0,	 0,	   0,	  0, 
				0,	0,	0,	0,	0,	 0,  -xy,	-yy, 
				0,	0,	0,	0,	0,	 0,    0,	  0, 
				0,	0,	0,	0,	0,	 0, -p.x(),   -p.y(), 
			    0,-xx,  0,-xy,  0,-p.x(),2*xxy,  2*yyx,
			    0,-xy,  0,-yy,  0,-p.y(),2*yyx,  2*yyy;
	}

	template <typename T>
	inline void PoseHomography<T>::apply( const ParameterVectorType& delta )
	{
		MatrixType dH;
		fillDeltaMatrix( dH, delta );
		_current *= dH;// * _current;
	}

	template <typename T>
	inline void PoseHomography<T>::applyInverse( const ParameterVectorType& delta )
	{
		MatrixType dH;
		fillDeltaMatrix( dH, delta );

//		if( !dH.inverseSelf() ){
//			throw CVTException( "Could not invert delta Matrix" );
//		}

		_current = dH.inverse() * _current;
	}


	template <typename T>
	inline void PoseHomography<T>::fillDeltaMatrix( MatrixType & deltaH, const ParameterVectorType& p ) const
	{
		deltaH( 0 , 0 ) = 1 + p[ 0 ];
		deltaH( 0 , 1 ) = p[ 2 ];
		deltaH( 0 , 2 ) = p[ 4 ];
		deltaH( 1 , 0 ) = p[ 1 ];
		deltaH( 1 , 1 ) = 1 + p[ 3 ];
		deltaH( 1 , 2 ) = p[ 5 ];
		deltaH( 2 , 0 ) = p[ 6 ];
		deltaH( 2 , 1 ) = p[ 7 ];
		deltaH( 2 , 2 ) = 1;
	}


	template <typename T>	
	inline void PoseHomography<T>::inverseCompositionalUpdate( const ParameterVectorType& delta )
	{
		applyInverse( delta );
	}

}

#endif
