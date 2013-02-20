/*
            CVT - Computer Vision Tools Library

     Copyright (c) 2012, Philipp Heise, Sebastian Klose

    THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
    KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
    IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
    PARTICULAR PURPOSE.
*/

#ifndef RGBDWARP_H
#define RGBDWARP_H

#include <cvt/math/SE3.h>
#include <cvt/util/SIMD.h>

namespace cvt
{

    class StandardWarp
    {
        public:
            EIGEN_MAKE_ALIGNED_OPERATOR_NEW
            static const size_t NParams = 6;
            typedef float                               Type;
            typedef Eigen::Matrix<Type, 1, NParams>     JacobianType;
            typedef Eigen::Matrix<Type, 2, NParams>     ScreenJacType;
            typedef Eigen::Matrix<Type, NParams, NParams> HessianType;
            typedef Eigen::Matrix<Type, NParams, 1>     DeltaVectorType;

            StandardWarp() {}
            ~StandardWarp(){}

            StandardWarp( const StandardWarp& other ) :
                _pose( other._pose )
            {}

            void initialize( const Matrix4f& pose )
            {
                setPose( pose );
            }

            void setPose( const Matrix4f& pose )
            {
                Eigen::Matrix<float, 4, 4> pe;
                EigenBridge::toEigen( pe, pose );
                _pose.set( pe );
            }

            Matrix4f pose() const
            {
                Matrix4f p;
                EigenBridge::toCVT( p, _pose.transformation() );
                return p;
            }

            static void screenJacobian( ScreenJacType& j,
                                        const Vector3f& point,
                                        const Matrix3f& K )
            {
                SE3<float>::screenJacobian( j, point, K );
            }

            void computeJacobian( JacobianType& j,
                                  const Vector3f& point,
                                  const Matrix3f& K,
                                  const Eigen::Matrix<float, 2, 1>& g,
                                  float /* pixval */ ) const
            {
                ScreenJacType J;
                screenJacobian( J, point, K );
                j = g.transpose() * J;
            }

            float computeResidual( float templateValue, float warpedValue ) const
            {
                return templateValue - warpedValue;
            }

            void computeResiduals( float* residuals, const float* referenceValues, const float* warped, size_t n ) const
            {
                SIMD::instance()->Sub( residuals, referenceValues, warped, n );
            }

            float costs( const float* residuals, const std::vector<size_t>& indices ) const
            {
                float ssd = 0.0f;

                for( std::vector<size_t>::const_iterator it = indices.begin(), end = indices.end();
                     it != end;
                     ++it ){
                    ssd += Math::sqr( residuals[ *it ] );
                }

                if( indices.size() )
                    return ssd / indices.size();
                else
                    return 1.0f;
            }

            void updateParameters( const DeltaVectorType& v )
            {
                _pose.applyInverse( -v );
            }

        private:
            SE3<Type>  _pose;
    };


    template <class T>
    class AffineLightingWarp
    {
        public:
            EIGEN_MAKE_ALIGNED_OPERATOR_NEW
            static const size_t NumParameters = 8;
            static const size_t PoseParameters = 6;
            typedef T                                               Type;
            typedef Eigen::Matrix<T, 1, NumParameters>              JacobianType;
            typedef Eigen::Matrix<T, 2, NumParameters>              ScreenJacType;
            typedef Eigen::Matrix<T, 2, PoseParameters>             PoseScreenJacType;
            typedef Eigen::Matrix<T, NumParameters, NumParameters>  HessianType;
            typedef Eigen::Matrix<T, NumParameters, 1>              DeltaVectorType;

            AffineLightingWarp() :
                _alpha( 0.0 ),
                _beta( 0.0 )
            {}

            ~AffineLightingWarp(){}

            AffineLightingWarp( const AffineLightingWarp& other ) :
                _pose( other._pose ),
                _alpha( other._alpha ),
                _beta( other._beta )
            {}

            AffineLightingWarp& operator= ( const AffineLightingWarp& other )
            {
                _pose = other._pose;
                _alpha = other._alpha;
                _beta = other._beta;
                return *this;
            }

            void initialize( const Matrix4<T>& pose )
            {
                setPose( pose );
                _alpha = 0.0;
                _beta = 0.0;
            }

            void setPose( const Matrix4<T>& pose )
            {
                Eigen::Matrix<T, 4, 4> pe;
                EigenBridge::toEigen( pe, pose );
                _pose.set( pe );
            }

            Matrix4<T> poseMatrix() const
            {
                Matrix4<T> p;
                EigenBridge::toCVT( p, _pose.transformation() );
                return p;
            }

            void updateParameters( const DeltaVectorType& v )
            {
                _pose.applyInverse( -v.template head<6>() );
                T ta = 1.0 + v[ 6 ];
                _alpha = ( _alpha - v[ 6 ] ) / ta;
                _beta  = ( _beta  - v[ 7 ] ) / ta;
            }

            static void screenJacobian( ScreenJacType& j,
                                        const Vector3<T>& point,
                                        const Matrix3<T>& K )
            {
                throw CVTException( "Affine Illumination needs refactoring before using like this" );
                //SE3<T>::screenJacobian( j, point, K );
            }

            static void computeJacobian( JacobianType& j,
                                         const Vector3<T>& point,
                                         const Matrix3<T>& K,
                                         const Eigen::Matrix<T, 2, 1>& g,
                                         float pixval )
            {
                typename SE3<T>::ScreenJacType J;
                SE3<T>::screenJacobian( J, point, K );
                j.template head<6>() = g.transpose() * J;
                j[ 6 ] = pixval;
                j[ 7 ] = 1;
            }

            float computeResidual( float templateValue, float warpedValue ) const
            {
                return templateValue - ( 1.0f + _alpha ) * warpedValue - _beta;
            }

            void computeResiduals( float* residuals, const float* referenceValues, const float* warped, size_t n )
            {
                SIMD* simd = SIMD::instance();
                simd->SubValue1f( residuals, referenceValues, _beta, n );
                simd->MulSubValue1f( residuals, warped, ( 1.0f + _alpha ), n );
            }

            float costs( const float* residuals, const std::vector<size_t>& indices ) const
            {
                float ssd = 0.0f;

                for( std::vector<size_t>::const_iterator it = indices.begin(), end = indices.end();
                     it != end;
                     ++it ){
                    ssd += Math::sqr( residuals[ *it ] );
                }

                if( indices.size() )
                    return ssd / indices.size();
                else
                    return 1.0f;
            }

        private:
            SE3<T>  _pose;
            T       _alpha;
            T       _beta;
    };
}

#endif // RGBDWARP_H
