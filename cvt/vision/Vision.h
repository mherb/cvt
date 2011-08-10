/*
 * File:   Vision.h
 * Author: sebi
 *
 * Created on August 9, 2011, 10:35 AM
 */

#ifndef CVT_VISION_H
#define	CVT_VISION_H

#include <cvt/math/Matrix.h>

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
        template <typename T>
        static void decomposeEssential( Matrix3<T> & R0,
                                        Matrix3<T> & R1,
                                        Vector3<T> & t0,
                                        Vector3<T> & t1,
                                        const Matrix3<T>& essential );

        template <typename T>
        static void triangulate( Vector4<T> & point3d,
                                 const Matrix4<T>& P0,
                                 const Matrix4<T>& P1,
                                 const Vector2<T>& p0,
                                 const Vector2<T>& p1 );
    };


    template <typename T>
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


    template <typename T>
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


}

#endif

