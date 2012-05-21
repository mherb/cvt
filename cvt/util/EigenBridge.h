/*
			CVT - Computer Vision Tools Library

 	 Copyright (c) 2012, Philipp Heise, Sebastian Klose

 	THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 	KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 	IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 	PARTICULAR PURPOSE.
 */
#ifndef CVT_EIGEN_BRIDGE_H
#define CVT_EIGEN_BRIDGE_H

#include <Eigen/Core>
#include <cvt/math/Vector.h>
#include <cvt/math/Matrix.h>

namespace cvt
{
	class EigenBridge {
		public:
			template <typename T0, typename T1>
				static void toEigen( Eigen::Matrix<T0, 4, 4> & e, const Matrix4<T1> & c );
			template <typename T0, typename T1>
				static void toEigen( Eigen::Matrix<T0, 3, 3> & e, const Matrix3<T1> & c );
			template <typename T0, typename T1>
				static void toEigen( Eigen::Matrix<T0, 2, 2> & e, const Matrix2<T1> & c );
			template <typename T0, typename T1>
				static void toEigen( Eigen::Matrix<T0, 2, 1> & e, const Vector2<T1> & c );
			template <typename T0, typename T1>
				static void toEigen( Eigen::Matrix<T0, 3, 1> & e, const Vector3<T1> & c );
			template <typename T0, typename T1>
				static void toEigen( Eigen::Matrix<T0, 4, 1> & e, const Vector4<T1> & c );

			template <typename T0, typename T1>
				static void toCVT( Matrix4<T0> & c, const Eigen::Matrix<T1, 4, 4> & e );
			template <typename T0, typename T1>
				static void toCVT( Matrix3<T0> & c, const Eigen::Matrix<T1, 3, 3> & e );
			template <typename T0, typename T1>
				static void toCVT( Matrix2<T0> & c, const Eigen::Matrix<T1, 2, 2> & e );
			template <typename T0, typename T1>
				static void toCVT( Vector2<T0> & c, const Eigen::Matrix<T1, 2, 1> & e );
			template <typename T0, typename T1>
				static void toCVT( Vector3<T0> & c, const Eigen::Matrix<T1, 3, 1> & e );
			template <typename T0, typename T1>
				static void toCVT( Vector4<T0> & c, const Eigen::Matrix<T1, 4, 1> & e );
	};

	template <typename T0, typename T1>
	inline void EigenBridge::toEigen( Eigen::Matrix<T0, 4, 4> & e, const Matrix4<T1> & c ) 
	{
		e( 0, 0 ) = c[ 0 ][ 0 ]; e( 0, 1 ) = c[ 0 ][ 1 ]; e( 0, 2 ) = c[ 0 ][ 2 ]; e( 0, 3 ) = c[ 0 ][ 3 ];
		e( 1, 0 ) = c[ 1 ][ 0 ]; e( 1, 1 ) = c[ 1 ][ 1 ]; e( 1, 2 ) = c[ 1 ][ 2 ]; e( 1, 3 ) = c[ 1 ][ 3 ];
		e( 2, 0 ) = c[ 2 ][ 0 ]; e( 2, 1 ) = c[ 2 ][ 1 ]; e( 2, 2 ) = c[ 2 ][ 2 ]; e( 2, 3 ) = c[ 2 ][ 3 ];
		e( 3, 0 ) = c[ 3 ][ 0 ]; e( 3, 1 ) = c[ 3 ][ 1 ]; e( 3, 2 ) = c[ 3 ][ 2 ]; e( 3, 3 ) = c[ 3 ][ 3 ];
	}

	template <typename T0, typename T1>
	inline void EigenBridge::toEigen( Eigen::Matrix<T0, 3, 3> & e, const Matrix3<T1> & c ) 
	{
		e( 0, 0 ) = c[ 0 ][ 0 ]; e( 0, 1 ) = c[ 0 ][ 1 ]; e( 0, 2 ) = c[ 0 ][ 2 ];
		e( 1, 0 ) = c[ 1 ][ 0 ]; e( 1, 1 ) = c[ 1 ][ 1 ]; e( 1, 2 ) = c[ 1 ][ 2 ];
		e( 2, 0 ) = c[ 2 ][ 0 ]; e( 2, 1 ) = c[ 2 ][ 1 ]; e( 2, 2 ) = c[ 2 ][ 2 ];
	}

	template <typename T0, typename T1>
	inline void EigenBridge::toEigen( Eigen::Matrix<T0, 2, 2> & e, const Matrix2<T1> & c )
	{
		e( 0, 0 ) = c[ 0 ][ 0 ]; e( 0, 1 ) = c[ 0 ][ 1 ]; 
		e( 1, 0 ) = c[ 1 ][ 0 ]; e( 1, 1 ) = c[ 1 ][ 1 ]; 
	}

	template <typename T0, typename T1>
	inline void EigenBridge::toEigen( Eigen::Matrix<T0, 2, 1> & e, const Vector2<T1> & c )
	{
		e[ 0 ] = c[ 0 ];
		e[ 1 ] = c[ 1 ];
	}

        template <typename T0, typename T1>
        inline void EigenBridge::toEigen( Eigen::Matrix<T0, 3, 1> & e, const Vector3<T1> & c )
        {
                e[ 0 ] = c[ 0 ];
                e[ 1 ] = c[ 1 ];
                e[ 2 ] = c[ 2 ];
        }
	
	template <typename T0, typename T1>
	inline void EigenBridge::toEigen( Eigen::Matrix<T0, 4, 1> & e, const Vector4<T1> & c )
	{
		e[ 0 ] = c[ 0 ];
		e[ 1 ] = c[ 1 ];
		e[ 2 ] = c[ 2 ];
		e[ 3 ] = c[ 3 ];
	}

	template <typename T0, typename T1>
	inline void EigenBridge::toCVT( Matrix4<T0> & c, const Eigen::Matrix<T1, 4, 4> & e ) 
	{
		c[ 0 ][ 0 ] = e( 0, 0 ); c[ 0 ][ 1 ] = e( 0, 1 ); c[ 0 ][ 2 ] = e( 0, 2 ); c[ 0 ][ 3 ] = e( 0, 3 );
		c[ 1 ][ 0 ] = e( 1, 0 ); c[ 1 ][ 1 ] = e( 1, 1 ); c[ 1 ][ 2 ] = e( 1, 2 ); c[ 1 ][ 3 ] = e( 1, 3 );
		c[ 2 ][ 0 ] = e( 2, 0 ); c[ 2 ][ 1 ] = e( 2, 1 ); c[ 2 ][ 2 ] = e( 2, 2 ); c[ 2 ][ 3 ] = e( 2, 3 );
		c[ 3 ][ 0 ] = e( 3, 0 ); c[ 3 ][ 1 ] = e( 3, 1 ); c[ 3 ][ 2 ] = e( 3, 2 ); c[ 3 ][ 3 ] = e( 3, 3 );
	}

	template <typename T0, typename T1>
	inline void EigenBridge::toCVT( Matrix3<T0> & c, const Eigen::Matrix<T1, 3, 3> & e ) 
	{
		c[ 0 ][ 0 ] = e( 0, 0 ); c[ 0 ][ 1 ] = e( 0, 1 ); c[ 0 ][ 2 ] = e( 0, 2 ); 
		c[ 1 ][ 0 ] = e( 1, 0 ); c[ 1 ][ 1 ] = e( 1, 1 ); c[ 1 ][ 2 ] = e( 1, 2 ); 
		c[ 2 ][ 0 ] = e( 2, 0 ); c[ 2 ][ 1 ] = e( 2, 1 ); c[ 2 ][ 2 ] = e( 2, 2 ); 
	}

	template <typename T0, typename T1>
	inline void EigenBridge::toCVT( Matrix2<T0> & c, const Eigen::Matrix<T1, 2, 2> & e )
	{
		c[ 0 ][ 0 ] = e( 0, 0 ); c[ 0 ][ 1 ] = e( 0, 1 ); 
		c[ 1 ][ 0 ] = e( 1, 0 ); c[ 1 ][ 1 ] = e( 1, 1 ); 
	}
	
	template <typename T0, typename T1>
	inline void EigenBridge::toCVT( Vector2<T0> & c, const Eigen::Matrix<T1, 2, 1> & e ) 
	{
		c[ 0 ] = e[ 0 ];
		c[ 1 ] = e[ 1 ];
	}
	
	template <typename T0, typename T1>
	inline void EigenBridge::toCVT( Vector3<T0> & c, const Eigen::Matrix<T1, 3, 1> & e ) 
	{
		c[ 0 ] = e[ 0 ];
		c[ 1 ] = e[ 1 ];
		c[ 2 ] = e[ 2 ];
	}

	template <typename T0, typename T1>
	inline void EigenBridge::toCVT( Vector4<T0> & c, const Eigen::Matrix<T1, 4, 1> & e ) 
	{
		c[ 0 ] = e[ 0 ];
		c[ 1 ] = e[ 1 ];
		c[ 2 ] = e[ 2 ];
		c[ 3 ] = e[ 3 ];
	}
}

#endif
