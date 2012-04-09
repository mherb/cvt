/*
			CVT - Computer Vision Tools Library

 	 Copyright (c) 2012, Philipp Heise, Sebastian Klose

 	THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 	KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 	IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 	PARTICULAR PURPOSE.
 */
/*
 * File:   Vision.h
 * Author: sebi
 *
 * Created on August 9, 2011, 10:35 AM
 */

#ifndef CVT_VISION_H
#define	CVT_VISION_H

#include <cvt/math/Matrix.h>
#include <cvt/gfx/Image.h>
#include <cvt/vision/CameraCalibration.h>
#include <cvt/geom/scene/ScenePoints.h>

#include <Eigen/SVD>

namespace cvt
{

    class Vision
    {
      public:

        /**
         * @brief Decomposition of Essential Matrix into R and t
         * As described in Multiple View Geometry p.258ff
         *
         * @param R0        first possible Rotation Matrix
         * @param R1        second possible Rotation Matrix
         * @param t0        first possible translation
         * @param t1        second possible translation
         * @param essential input essential matrix
         *
         */
        template<typename T>
        static void decomposeEssential( Matrix3<T> & R0,
                                        Matrix3<T> & R1,
                                        Vector3<T> & t0,
                                        Vector3<T> & t1,
                                        const Matrix3<T>& essential );

        template<typename T>
        static void triangulate( Vector4<T> & point3d,
                                 const Matrix4<T>& P0,
                                 const Matrix4<T>& P1,
                                 const Vector2<T>& p0,
                                 const Vector2<T>& p1 );

		/**
		 *	@brief	Construct fundamental matrix (cam 0 to cam1) out of calibration data
		 *	@param	f	output fundamental matrix
		 *	@param	K0	intrinsics of camera 0
		 *	@param	T0	extrinsics of camera 0 (cam2World)
		 *	@param	K1	intrinsics of camera 1
		 *	@param	T1	extrinsics of camera 1 (cam2World)
		 */
		template<typename T>
		static void composeFundamental( Matrix3<T>& f, 
									    const Matrix3<T>& K0, 
										const Matrix4<T>& T0,
										const Matrix3<T>& K1, 
										const Matrix4<T>& T1 );

		/**
		  @brief Extract the epipoles of a fundamental matrix
		  @param e0 epipole such that e0 * F^T = 0;
		  @param e1 epipole such that F * e1 = 0;
		  @param f  fundamental matrix
		*/
		template<typename T>
		static void epipolesFundamental( Vector3<T>& e0, Vector3<T>& e1, Matrix3<T>& f );

		/*
		  @brief  A. Fusiello, E. Trucco, and A. Verri. A compact algorithm for rectification of stereo pairs. Machine Vision and Applications, 12(1):16-22, 2000.
		*/
		template<typename T>
		static void stereoRectification( Matrix3<T>& K0new,
										 Matrix4<T>& T0new,
										 Matrix3<T>& K1new,
										 Matrix4<T>& T1new,
										 Matrix3<T>& Timage0,
										 Matrix3<T>& Timage1,
										 const Matrix3<T>& K0,
										 const Matrix4<T>& T0,
										 const Matrix3<T>& K1,
										 const Matrix4<T>& T1 );
		/**
		 *	@brief	correct point correspondences according to fundamental constraint
		 *			using first order Sampson Approximation
		 *	@param p0			Point correspondence 0
		 *	@param p1			Point correspondence 1
		 *	@param fundamental	Fundamental Matrix: p1'Fp0 = 0
		 */
		template<typename T>
		static void correctCorrespondencesSampson( Vector2<T>& p0, 
												   Vector2<T>& p1,
												   const Matrix3<T>& fundamental );

	/*	template<typename T>
		static void correctCorrespondences( Vector3<T>& p0,
										    Vector3<T>& p1,
										    const Matrix3<T>& essential );*/

		/**
		 *	@brief	project 3d point to a screen point with intrinsic matrix
		 *	@param	p2	resulting screenpoint
		 *	@param	K	intrinsic matrix
		 *	@param	p3	3d point given in camera coordinates
		 */
		template <class Mat3, class Vec3, class Vec2>
		static void project( Vec2 & p2, const Mat3 & K, const Vec3 & p3 );
		
		template <class Mat3, class Vec3, class Vec2>
		static void project( Vec2 & p2, const Mat3 & KR, const Vec3 & Kt, const Vec3 & p3 );
		
		template <class T>
		static void project( Eigen::Matrix<T, 2, 1> & p2, 
						     const Eigen::Matrix<T, 3, 3>& K, 
							 const Eigen::Matrix<T, 4, 4>& trans, 
							 const Eigen::Matrix<T, 4, 1>& p );

		static void unprojectToScenePoints( ScenePoints& pts, const Image& texture, const Image& depthmap, const CameraCalibration& calibration, float dscale = 1.0f );

		static void unprojectToScenePoints( ScenePoints& pts, const Image& texture, const Image& depthmap, float dscale = 1.0f );
    };


    template<typename T>
    inline void Vision::decomposeEssential( Matrix3<T> & R0,
                                            Matrix3<T> & R1,
                                            Vector3<T> & t0,
                                            Vector3<T> & t1,
                                            const Matrix3<T>& essential )
    {
        Eigen::Matrix<T, 3, 3> E;
        for( size_t i = 0; i < 3; i++ )
            for( size_t k = 0; k < 3; k++ )
                E( i, k ) = essential[ i ][ k ];

        Eigen::JacobiSVD<Eigen::Matrix<T, 3, 3> > svd( E, Eigen::ComputeFullU | Eigen::ComputeFullV );

        // compute the results:
        Eigen::Matrix<T, 3, 3> W( Eigen::Matrix<T, 3, 3>::Zero() );
        Eigen::Matrix<T, 3, 3> Z( Eigen::Matrix<T, 3, 3>::Zero() );

        W( 0, 1 ) = -1;
        W( 1, 0 ) =  1;
        W( 2, 2 ) =  1;

        Z( 0, 1 ) =  1;
        Z( 1, 0 ) = -1;

        Eigen::Matrix<T, 3, 3> S = svd.matrixU() * Z * svd.matrixU().transpose();

        t0[ 0 ] = -S( 1, 2 );
        t0[ 1 ] =  S( 0, 2 );
        t0[ 2 ] = -S( 0, 1 );
        t1 = -t0;

        // case 1: R = UWV'
        S = svd.matrixU() * W * svd.matrixV().transpose();
        if( S.determinant() < 0 )
            S = -S;
        for( size_t i = 0; i < 3; i++ )
            for( size_t k = 0; k < 3; k++ )
                R0[ i ][ k ] = S( i, k );


        S = svd.matrixU() * W.transpose() * svd.matrixV().transpose();
        if( S.determinant() < 0 )
            S = -S;

        for( size_t i = 0; i < 3; i++ )
            for( size_t k = 0; k < 3; k++ )
                R1[ i ][ k ] = S( i, k );
    }


    template<typename T>
    inline void Vision::triangulate( Vector4<T> & point3d,
                                     const Matrix4<T>& proj0,
                                     const Matrix4<T>& proj1,
                                     const Vector2<T>& p0,
                                     const Vector2<T>& p1 )
    {
        Eigen::Matrix<T, 4, 4> A;

        A( 0, 0 ) = p0.x * proj0[ 2 ][ 0 ] - proj0[ 0 ][ 0 ];
        A( 0, 1 ) = p0.x * proj0[ 2 ][ 1 ] - proj0[ 0 ][ 1 ];
        A( 0, 2 ) = p0.x * proj0[ 2 ][ 2 ] - proj0[ 0 ][ 2 ];
        A( 0, 3 ) = p0.x * proj0[ 2 ][ 3 ] - proj0[ 0 ][ 3 ];

        A( 1, 0 ) = p0.y * proj0[ 2 ][ 0 ] - proj0[ 1 ][ 0 ];
        A( 1, 1 ) = p0.y * proj0[ 2 ][ 1 ] - proj0[ 1 ][ 1 ];
        A( 1, 2 ) = p0.y * proj0[ 2 ][ 2 ] - proj0[ 1 ][ 2 ];
        A( 1, 3 ) = p0.y * proj0[ 2 ][ 3 ] - proj0[ 1 ][ 3 ];

        A( 2, 0 ) = p1.x * proj1[ 2 ][ 0 ] - proj1[ 0 ][ 0 ];
        A( 2, 1 ) = p1.x * proj1[ 2 ][ 1 ] - proj1[ 0 ][ 1 ];
        A( 2, 2 ) = p1.x * proj1[ 2 ][ 2 ] - proj1[ 0 ][ 2 ];
        A( 2, 3 ) = p1.x * proj1[ 2 ][ 3 ] - proj1[ 0 ][ 3 ];

        A( 3, 0 ) = p1.y * proj1[ 2 ][ 0 ] - proj1[ 1 ][ 0 ];
        A( 3, 1 ) = p1.y * proj1[ 2 ][ 1 ] - proj1[ 1 ][ 1 ];
        A( 3, 2 ) = p1.y * proj1[ 2 ][ 2 ] - proj1[ 1 ][ 2 ];
        A( 3, 3 ) = p1.y * proj1[ 2 ][ 3 ] - proj1[ 1 ][ 3 ];

        Eigen::JacobiSVD<Eigen::Matrix<T, 4, 4> > svd( A, Eigen::ComputeFullU | Eigen::ComputeFullV );

        const Eigen::Matrix<T, 4, 1> & v = svd.matrixV().col( 3 );
        point3d[ 0 ] = v[ 0 ];
        point3d[ 1 ] = v[ 1 ];
        point3d[ 2 ] = v[ 2 ];
        point3d[ 3 ] = v[ 3 ];
    }

	template<typename T>
	inline void Vision::correctCorrespondencesSampson( Vector2<T>& p0, 
													   Vector2<T>& p1,
													   const Matrix3<T>& fundamental )
	{
		Vector3<T> x0, x1;
		x0.x = p0.x;
		x0.y = p0.y;
		x0.z = 1;
		x1.x = p1.x;
		x1.y = p1.y;
		x1.z = 1;
		
		Vector3<T> Fx0 = fundamental * x0; 
		Vector3<T> FTx1 = fundamental.transpose() * x1;

		T factor = x1*Fx0 / 
				   ( Math::sqr( Fx0[ 0 ] ) + 
					 Math::sqr( Fx0[ 1 ] ) + 
					 Math::sqr( FTx1[ 0 ] ) + 
					 Math::sqr( FTx1[ 1 ] ) );
		p0[ 0 ] -= factor * FTx1[ 0 ];
		p0[ 1 ] -= factor * FTx1[ 1 ];
		p1[ 0 ] -= factor * Fx0[ 0 ];
		p1[ 1 ] -= factor * Fx0[ 1 ];
	}

#if 0
	template<typename T>
	inline void Vision::correctCorrespondences( Vector3<T>& p0,
										        Vector3<T>& p1,
												const Matrix3<T>& essential )
	{
		Matrix3<T> T0, T1, R0, R1;
		Matrix3<T> E;
		Matrix3<T> U, D, V;
		Vector3<T> e0, e1;

		T0.setIdentity()
		T0[ 0 ][ 2 ] = p0.x;
		T0[ 1 ][ 2 ] = p0.y;
		T1.setIdentity();
		T1[ 0 ][ 2 ] = p1.x;
		T1[ 1 ][ 2 ] = p1.y;

		E = T1.transpose() * essential * T0;

		E.svd( U, D, V );

		e0.set( U[ 0 ][ 2 ],  U[ 1 ][ 2 ], U[ 2 ][ 2 ] );
		e0 /= Math::sqr( e0.x ) + Math::sqr( e0.y );

		e1 = V[ 2 ];
		e1 /= Math::sqr( e1.x ) + Math::sqr( e1.y );

		R0.setIdentity();
		R0[ 0 ][ 0 ] =  e0.x;
		R0[ 0 ][ 1 ] =  e0.y;
		R0[ 1 ][ 0 ] = -e0.y;
		R0[ 1 ][ 1 ] =  e0.x;

		R1.setIdentity();
		R1[ 0 ][ 0 ] =  e1.x;
		R1[ 0 ][ 1 ] =  e1.y;
		R1[ 1 ][ 0 ] = -e1.y;
		R1[ 1 ][ 1 ] =  e1.x;

		E = R1 * E * R0.transpose();


	}
#endif

		template<typename T>
		inline void Vision::composeFundamental( Matrix3<T>& f, 
												const Matrix3<T>& K0, 
												const Matrix4<T>& T0,
												const Matrix3<T>& K1, 
												const Matrix4<T>& T1 )
		{
			/* from cam1 to cam0 */
			Matrix4<T> t10 = T1 * T0.inverse();
			Matrix3<T> R = t10.toMatrix3();
			Matrix3<T> tSkew;
			tSkew.setSkewSymmetric( Vector3<T>( t10[ 0 ][ 3 ], t10[ 1 ][ 3 ], t10[ 2 ][ 3 ] ) );
			f = K1.inverse().transpose() * tSkew * R * K0.inverse();	
		}

		template<typename T>
		inline void Vision::epipolesFundamental( Vector3<T>& e0, Vector3<T>& e1, Matrix3<T>& f )
		{
			Matrix3<T> U, D, V;
			f.svd( U, D, V );
			e0 = U.col( 2 );
			e1 = V.row( 2 );
		}


		template<typename T>
		inline void Vision::stereoRectification( Matrix3<T>& K0new,
												 Matrix4<T>& T0new,
												 Matrix3<T>& K1new,
												 Matrix4<T>& T1new,
												 Matrix3<T>& Timage0,
												 Matrix3<T>& Timage1,
												 const Matrix3<T>& K0,
												 const Matrix4<T>& T0,
												 const Matrix3<T>& K1,
												 const Matrix4<T>& T1 )
		{
			Vector3<T> c0, c1, v1, v2, v3;

			c0 = -Matrix3<T>( T0 ).transpose() * Vector3<T>( T0[ 0 ][ 3 ], T0[ 1 ][ 3 ], T0[ 2 ][ 3 ] );
			c1 = -Matrix3<T>( T1 ).transpose() * Vector3<T>( T1[ 0 ][ 3 ], T1[ 1 ][ 3 ], T1[ 2 ][ 3 ] );

			v1 = c1 - c0;
			v2 = Vector3<T>( T0[ 2 ][ 0 ], T0[ 2 ][ 1 ], T0[ 2 ][ 2 ] ).cross( v1 );
			v3 = v1.cross( v2 );

			Matrix3<T> R( v1 / v1.length(), v2 / v2.length(), v3 / v3.length() );

			K0new = ( K0 + K1 ) * 0.5f;
			K0new[ 0 ][ 1 ] = 0.0f; // no skew
			K1new = K0new;

//			Matrix3<T> K01new2 = K01new;
//			K01new[ 0 ][ 2 ] = K0[ 0 ][ 2 ];
//			K01new2[ 0 ][ 2 ] = K1[ 0 ][ 2 ];

			T0new = R.toMatrix4();
			T1new = R.toMatrix4();

			Vector3<T> tmp = - R * c0;
			T0new[ 0 ][ 3 ] = tmp[ 0 ];
			T0new[ 1 ][ 3 ] = tmp[ 1 ];
			T0new[ 2 ][ 3 ] = tmp[ 2 ];

			tmp = - R * c1;
			T1new[ 0 ][ 3 ] = tmp[ 0 ];
			T1new[ 1 ][ 3 ] = tmp[ 1 ];
			T1new[ 2 ][ 3 ] = tmp[ 2 ];

			Timage0 = K0new * T0new.toMatrix3() * ( K0 * T0.toMatrix3() ).inverse();
			Timage1 = K1new * T1new.toMatrix3() * ( K1 * T1.toMatrix3() ).inverse();
		}

		template <class Mat3, class Vec3, class Vec2>
		inline void Vision::project( Vec2 & p2, const Mat3 & K, const Vec3 & p3 )
		{
			Vec3 pp = K * p3;
			p2[ 0 ] = pp[ 0 ] / pp[ 2 ];
			p2[ 1 ] = pp[ 1 ] / pp[ 2 ];
		}

		template <class Mat3, class Vec3, class Vec2>
		inline void Vision::project( Vec2 & p2, const Mat3 & KR, const Vec3 & Kt, const Vec3 & p3 )
		{
			Vec3 pp = KR * p3 + Kt;
			p2[ 0 ] = pp[ 0 ] / pp[ 2 ];
			p2[ 1 ] = pp[ 1 ] / pp[ 2 ];
		}

		template <class T>
		inline void Vision::project( Eigen::Matrix<T, 2, 1> & p2, 
									 const Eigen::Matrix<T, 3, 3>& K, 
									 const Eigen::Matrix<T, 4, 4>& trans, 
									 const Eigen::Matrix<T, 4, 1>& p )
		{
			Eigen::Matrix<T, 4, 1> ptmp = trans * p;
			
			Eigen::Matrix<T, 3, 1> p3;
			p3[ 0 ] = ptmp[ 0 ] / ptmp[ 3 ];
			p3[ 1 ] = ptmp[ 1 ] / ptmp[ 3 ];
			p3[ 2 ] = ptmp[ 2 ] / ptmp[ 3 ];
			p3 = K * p3;
			
			p2[ 0 ] = p3[ 0 ] / p3[ 2 ];
			p2[ 1 ] = p3[ 1 ] / p3[ 2 ];
		}

}

#endif

