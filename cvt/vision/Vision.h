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
        template <typename T>
        static void decomposeEssential( Matrix3<T> & R0,
                                        Matrix3<T> & R1,
                                        Vector3<T> & t0,
                                        Vector3<T> & t1,
                                        const Matrix3<T>& essential );

        template <typename T>
        static void computeDepths( Vector2<T> & lamdas,
                                   const Matrix3<T> & R,
                                   const Vector3<T> & t );
    };

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
    inline void Vision::computeDepths( Vector2<T> & lamdas,
                                       const Matrix3<T> & R,
                                       const Vector3<T> & t )
    {
        Vector3<T> p, pp;
        p[ 0 ] = 0;
        p[ 1 ] = 0;
        p[ 2 ] = 1;

        pp = R * p;

        // lamda0 * p = t + lamda1 * ( R*p )
        // lamda0 * p - lamda1 * pp = t
        //
        // px   -ppx    lamda0   tx
        // py   -ppy  * lamda1 = ty
        //  1   -ppx             tz

        Matrix2<T> A;
        Vector2<T> tt;
        A.setZero();

        A[ 0 ][ 0 ] = p * p;
        A[ 0 ][ 1 ] = -p * pp;
        A[ 1 ][ 0 ] = A[ 0 ][ 1 ];
        A[ 1 ][ 1 ] = pp * pp;

        tt[ 0 ] = p * t;
        tt[ 1 ] = -pp * t;

        if( !A.inverseSelf() ){
            throw CVTException( "Could not invert Matrix" );
        }

        lamdas = A * tt;
    }


}

#endif

