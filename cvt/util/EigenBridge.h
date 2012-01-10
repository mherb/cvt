#ifndef CVT_EIGEN_BRIDGE_H
#define CVT_EIGEN_BRIDGE_H

#include <Eigen/Core>
#include <cvt/math/Vector.h>
#include <cvt/math/Matrix.h>

namespace cvt
{
	class EigenBridge {
		public:
			template <typename T>
				static void toEigen( Eigen::Matrix<T, 4, 4> & e, const Matrix4<T> & c );
			template <typename T>
				static void toEigen( Eigen::Matrix<T, 3, 3> & e, const Matrix3<T> & c );
			template <typename T>
				static void toEigen( Eigen::Matrix<T, 2, 2> & e, const Matrix2<T> & c );
			template <typename T>
				static void toEigen( Eigen::Matrix<T, 2, 1> & e, const Vector2<T> & c );
			template <typename T>
				static void toEigen( Eigen::Matrix<T, 4, 1> & e, const Vector4<T> & c );

			template <typename T>
				static void toCVT( Matrix4<T> & c, const Eigen::Matrix<T, 4, 4> & e );
			template <typename T>
				static void toCVT( Matrix3<T> & c, const Eigen::Matrix<T, 3, 3> & e );
			template <typename T>
				static void toCVT( Matrix2<T> & c, const Eigen::Matrix<T, 2, 2> & e );
			template <typename T>
				static void toCVT( Vector2<T> & c, const Eigen::Matrix<T, 2, 1> & e );
			template <typename T>
				static void toCVT( Vector4<T> & c, const Eigen::Matrix<T, 4, 1> & e );
	};

	template <typename T>
	inline void EigenBridge::toEigen( Eigen::Matrix<T, 4, 4> & e, const Matrix4<T> & c ) 
	{
		e( 0, 0 ) = c[ 0 ][ 0 ]; e( 0, 1 ) = c[ 0 ][ 1 ]; e( 0, 2 ) = c[ 0 ][ 2 ]; e( 0, 3 ) = c[ 0 ][ 3 ];
		e( 1, 0 ) = c[ 1 ][ 0 ]; e( 1, 1 ) = c[ 1 ][ 1 ]; e( 1, 2 ) = c[ 1 ][ 2 ]; e( 1, 3 ) = c[ 1 ][ 3 ];
		e( 2, 0 ) = c[ 2 ][ 0 ]; e( 2, 1 ) = c[ 2 ][ 1 ]; e( 2, 2 ) = c[ 2 ][ 2 ]; e( 2, 3 ) = c[ 2 ][ 3 ];
		e( 3, 0 ) = c[ 3 ][ 0 ]; e( 3, 1 ) = c[ 3 ][ 1 ]; e( 3, 2 ) = c[ 3 ][ 2 ]; e( 3, 3 ) = c[ 3 ][ 3 ];
	}

	template <typename T>
	inline void EigenBridge::toEigen( Eigen::Matrix<T, 3, 3> & e, const Matrix3<T> & c ) 
	{
		e( 0, 0 ) = c[ 0 ][ 0 ]; e( 0, 1 ) = c[ 0 ][ 1 ]; e( 0, 2 ) = c[ 0 ][ 2 ];
		e( 1, 0 ) = c[ 1 ][ 0 ]; e( 1, 1 ) = c[ 1 ][ 1 ]; e( 1, 2 ) = c[ 1 ][ 2 ];
		e( 2, 0 ) = c[ 2 ][ 0 ]; e( 2, 1 ) = c[ 2 ][ 1 ]; e( 2, 2 ) = c[ 2 ][ 2 ];
	}

	template <typename T>
	inline void EigenBridge::toEigen( Eigen::Matrix<T, 2, 2> & e, const Matrix2<T> & c )
	{
		e( 0, 0 ) = c[ 0 ][ 0 ]; e( 0, 1 ) = c[ 0 ][ 1 ]; 
		e( 1, 0 ) = c[ 1 ][ 0 ]; e( 1, 1 ) = c[ 1 ][ 1 ]; 
	}

	template <typename T>
	inline void EigenBridge::toEigen( Eigen::Matrix<T, 2, 1> & e, const Vector2<T> & c )
	{
		e[ 0 ] = c[ 0 ];
		e[ 1 ] = c[ 1 ];
	}
	
	template <typename T>
	inline void EigenBridge::toEigen( Eigen::Matrix<T, 4, 1> & e, const Vector4<T> & c )
	{
		e[ 0 ] = c[ 0 ];
		e[ 1 ] = c[ 1 ];
		e[ 2 ] = c[ 2 ];
		e[ 3 ] = c[ 3 ];
	}

	template <typename T>
	inline void EigenBridge::toCVT( Matrix4<T> & c, const Eigen::Matrix<T, 4, 4> & e ) 
	{
		c[ 0 ][ 0 ] = e( 0, 0 ); c[ 0 ][ 1 ] = e( 0, 1 ); c[ 0 ][ 2 ] = e( 0, 2 ); c[ 0 ][ 3 ] = e( 0, 3 );
		c[ 1 ][ 0 ] = e( 1, 0 ); c[ 1 ][ 1 ] = e( 1, 1 ); c[ 1 ][ 2 ] = e( 1, 2 ); c[ 1 ][ 3 ] = e( 1, 3 );
		c[ 2 ][ 0 ] = e( 2, 0 ); c[ 2 ][ 1 ] = e( 2, 1 ); c[ 2 ][ 2 ] = e( 2, 2 ); c[ 2 ][ 3 ] = e( 2, 3 );
		c[ 3 ][ 0 ] = e( 3, 0 ); c[ 3 ][ 1 ] = e( 3, 1 ); c[ 3 ][ 2 ] = e( 3, 2 ); c[ 3 ][ 3 ] = e( 3, 3 );
	}

	template <typename T>
	inline void EigenBridge::toCVT( Matrix3<T> & c, const Eigen::Matrix<T, 3, 3> & e ) 
	{
		c[ 0 ][ 0 ] = e( 0, 0 ); c[ 0 ][ 1 ] = e( 0, 1 ); c[ 0 ][ 2 ] = e( 0, 2 ); 
		c[ 1 ][ 0 ] = e( 1, 0 ); c[ 1 ][ 1 ] = e( 1, 1 ); c[ 1 ][ 2 ] = e( 1, 2 ); 
		c[ 2 ][ 0 ] = e( 2, 0 ); c[ 2 ][ 1 ] = e( 2, 1 ); c[ 2 ][ 2 ] = e( 2, 2 ); 
	}

	template <typename T>
	inline void EigenBridge::toCVT( Matrix2<T> & c, const Eigen::Matrix<T, 2, 2> & e )
	{
		c[ 0 ][ 0 ] = e( 0, 0 ); c[ 0 ][ 1 ] = e( 0, 1 ); 
		c[ 1 ][ 0 ] = e( 1, 0 ); c[ 1 ][ 1 ] = e( 1, 1 ); 
	}
	
	template <typename T>
	inline void EigenBridge::toCVT( Vector2<T> & c, const Eigen::Matrix<T, 2, 1> & e ) 
	{
		c[ 0 ] = e[ 0 ];
		c[ 1 ] = e[ 1 ];
	}
	
	template <typename T>
	inline void EigenBridge::toCVT( Vector4<T> & c, const Eigen::Matrix<T, 4, 1> & e ) 
	{
		c[ 0 ] = e[ 0 ];
		c[ 1 ] = e[ 1 ];
		c[ 2 ] = e[ 2 ];
		c[ 3 ] = e[ 3 ];
	}
}

#endif
