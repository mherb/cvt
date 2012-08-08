/*
            CVT - Computer Vision Tools Library

     Copyright (c) 2012, Philipp Heise, Sebastian Klose

    THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
    KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
    IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
    PARTICULAR PURPOSE.
*/

#ifndef RGBDKEYFRAME_H
#define RGBDKEYFRAME_H

#include <cvt/math/Matrix.h>
#include <cvt/math/SE3.h>
#include <cvt/vision/ImagePyramid.h>
#include <cvt/gfx/IMapScoped.h>
#include <cvt/util/EigenBridge.h>

#include <cvt/vision/rgbdvo/RGBDWarp.h>
#include <cvt/vision/rgbdvo/RobustWeighting.h>

#include<Eigen/StdVector>

namespace cvt
{
    template <class WarpFunc,
              class Weighter>
    class RGBDKeyframe {
        public:
            EIGEN_MAKE_ALIGNED_OPERATOR_NEW
            struct Result {
                Result() :
                    success( false ),
                    iterations( 0 ),
                    numPixels( 0 ),
                    pixelPercentage( 0.0f ),
                    costs( 0.0f )
                {
                }

                bool        success;
                size_t      iterations;
                size_t      numPixels;
                float       pixelPercentage; /* between 0 and 1 */
                float       costs;
                WarpFunc    warp;
            };

            typedef typename WarpFunc::Type  T;
            typedef Vector3<T> PointType;
            typedef Matrix3<T> Mat3Type;

            typedef typename WarpFunc::JacobianType  JacobianType;
            typedef typename WarpFunc::HessianType   HessianType;
            typedef WarpFunc                         WarpFunction;

            struct AlignmentData {
                    EIGEN_MAKE_ALIGNED_OPERATOR_NEW

                    std::vector<PointType>      points3d;
                    std::vector<float>          pixelValues;
                    std::vector<JacobianType>   jacobians;

                    HessianType                 hessian;
                    HessianType                 inverseHessian;

                    Mat3Type                    intrinsics;

                    void reserve( size_t size )
                    {
                        points3d.reserve( size );
                        pixelValues.reserve( size );
                        jacobians.reserve( size );
                    }

                    void clear()
                    {
                        points3d.clear();
                        pixelValues.clear();
                        jacobians.clear();
                    }
            };

            RGBDKeyframe( const Mat3Type &K, size_t octaves, float scale );
            virtual ~RGBDKeyframe();

            const AlignmentData& dataForScale( size_t o ) const { return _dataForScale[ o ]; }
            const Matrix4<T>&    pose()                   const { return _pose; }

            /* originally, the depth map is stored as uint16_t, and when we convert it to float it will be mapped between 0 and 1!*/
            void setDepthMapScaleFactor( float scaleFactor )        { _depthScaling = ( ( float )0xFFFF ) / scaleFactor; }
            void setMinimumDepth( T depthTresh )                    { _minDepth = depthTresh; }
            void setGradientThreshold( float thresh )               { _gradientThreshold = thresh; }
            void setMaxIter( size_t maxiter )                       { _maxIters = maxiter; }
            void setMinUpdate( T minUpdate )                        { _minUpdate = minUpdate; }
            void setTranslationJumpThreshold( T maxTDiff )          { _translationJumpThreshold = maxTDiff; }
            void setMinPixelPercentage( float minPixelPercentage )  { _minPixelPercentage = minPixelPercentage; }
            void setRobustParam( T v )                              { _weighter = Weighter( v ); }

            /**
             *  \brief align the current camera frame with this keyframe
             *  \param prediction   predicted pose of the camera frame in world frame (T_wc)
             */
            void align( Result& result, const Matrix4<T>& prediction, const ImagePyramid& pyr, const Image& depth );

            virtual void updateOfflineData( const Matrix4<T>& pose, const ImagePyramid& pyramid, const Image& depth ) = 0;

        protected:
            Matrix4<T>                  _pose;

            typedef std::vector<AlignmentData, Eigen::aligned_allocator<AlignmentData> > AlignmentDataVector;
            AlignmentDataVector         _dataForScale;
            IKernel                     _kx;
            IKernel                     _ky;
            IKernel                     _gaussX;
            IKernel                     _gaussY;

            float                       _depthScaling;
            T                           _minDepth;
            float                       _gradientThreshold;
            size_t                      _maxIters;
            T                           _minUpdate;
            T                           _translationJumpThreshold;
            float                       _minPixelPercentage;

            Weighter                    _weighter;

            void updateIntrinsics( const Mat3Type& K, float scale );
            void computeImageGradients( Image& gx, Image& gy, const Image& gray ) const;

            void alignSingleScale( Result& result, const Image& gray, const Image& depth, size_t octave );
            bool checkResult( const Result& res, const Matrix4<T> &lastPose ) const;

            virtual void alignSingleScaleNonRobust( Result& result, const Image& gray, const Image& depth, size_t octave ) = 0;
            virtual void alignSingleScaleRobust( Result& result, const Image& gray, const Image& depth, size_t octave ) = 0;

            float interpolateDepth( const Vector2f& p, const float* ptr, size_t stride ) const;
            void  initializePointLookUps( float* vals, size_t n, float foc, float center ) const;
    };

    template <class WarpFunc, class Weighter>
    inline RGBDKeyframe<WarpFunc, Weighter>::RGBDKeyframe( const Mat3Type &K, size_t octaves, float scale ) :
        _kx( IKernel::HAAR_HORIZONTAL_3 ),
        _ky( IKernel::HAAR_VERTICAL_3 ),
        _gaussX( IKernel::GAUSS_HORIZONTAL_3 ),
        _gaussY( IKernel::GAUSS_VERTICAL_3 ),
        _depthScaling( 1.0f ),
        _minDepth( 0.05 ),
        _gradientThreshold( 0.0f ),
        _maxIters( 5 ),
        _minUpdate( (T)1e-6 ),
        _translationJumpThreshold( ( T )0.8 ),
        _minPixelPercentage( 0.2f ),
        _weighter( (T)0.1 )
    {
        float s = -0.5f;
        _kx.scale( s );
        _ky.scale( s );

        _dataForScale.resize( octaves );
        updateIntrinsics( K, scale );
    }

    template <class WarpFunc, class Weighter>
    inline RGBDKeyframe<WarpFunc, Weighter>::~RGBDKeyframe()
    {
    }

    template <class WarpFunc, class Weighter>
    inline void RGBDKeyframe<WarpFunc, Weighter>::updateIntrinsics( const Mat3Type& K, float scale )
    {
        _dataForScale[ 0 ].intrinsics = K;
        for( size_t o = 1; o < _dataForScale.size(); o++ ){
            _dataForScale[ o ].intrinsics = _dataForScale[ o - 1 ].intrinsics * ( T )scale;
            _dataForScale[ o ].intrinsics[ 2 ][ 2 ] = ( T )1.0;
        }
    }

    template <class WarpFunc, class Weighter>
    inline void RGBDKeyframe<WarpFunc, Weighter>::computeImageGradients( Image& gx, Image& gy, const Image& gray ) const
    {
        gx.reallocate( gray.width(), gray.height(), IFormat::GRAY_FLOAT );
        gy.reallocate( gray.width(), gray.height(), IFormat::GRAY_FLOAT );

        // sobel
        gray.convolve( gx, _kx, _gaussY );
        gray.convolve( gy, _gaussX, _ky );

        // normal
        //gray.convolve( gx, _kx );
        //gray.convolve( gy, _ky );
    }

    template <class WarpFunc, class Weighter>
    inline void RGBDKeyframe<WarpFunc, Weighter>::align( Result& result,
                                                         const Matrix4<T>& prediction,
                                                         const ImagePyramid& pyr, const Image &depth )
    {
        Matrix4<T> tmp4;
        tmp4 = prediction.inverse() * _pose;

        result.warp.setPose( tmp4 );
        result.costs = 0.0f;
        result.iterations = 0;
        result.numPixels = 0;
        result.pixelPercentage = 0.0f;

        Result scaleResult;
        scaleResult = result;
        for( int o = pyr.octaves() - 1; o >= 0; o-- ){
            alignSingleScale( scaleResult, pyr[ o ], depth, o );

            if( checkResult( scaleResult, result.warp.poseMatrix() ) ){
                // seems to be a good alignment
                result = scaleResult;
            } else {
                scaleResult = result;
            }
        }

        tmp4 = result.warp.poseMatrix();
        tmp4 = _pose * tmp4.inverse();
        result.warp.setPose( tmp4 );
    }

    template <class WarpFunc, class Weighter>
    inline void RGBDKeyframe<WarpFunc, Weighter>::alignSingleScale( Result& result, const Image& gray, const Image& depth, size_t octave )
    {
        if( IsRobustWeighting<Weighter>::Value ){
            alignSingleScaleRobust( result, gray, depth, octave );
        } else {
            alignSingleScaleNonRobust( result, gray, depth, octave );
        }
    }

    template <class WarpFunc, class Weighter>
    inline bool RGBDKeyframe<WarpFunc, Weighter>::checkResult( const Result& res, const Matrix4<T>& lastPose ) const
    {
        // to few pixels projected into image
        if( res.pixelPercentage < _minPixelPercentage ){
            //std::cout << "Pixel Percentage: " << res.pixelPercentage << " : " << _minPixelPercentage << std::endl;
            return false;
        }

        // jump
        Matrix4<T> mat = res.warp.poseMatrix();
        if( ( mat.col( 3 ) - lastPose.col( 3 ) ).length() > _translationJumpThreshold ){
            //std::cout << "Delta T: " << mat.col( 3 ) << " : " << lastPose.col( 3 ) << std::endl;
            return false;
        }

        return true;
    }

    template <class WarpFunc, class Weighter>
    inline float RGBDKeyframe<WarpFunc, Weighter>::interpolateDepth( const Vector2f& p, const float* ptr, size_t stride ) const
    {
        // get the fractions:
        int xi = p.x;
        int yi = p.y;

        const float* curr = ptr + yi * stride + xi ;

        float wx = p.x - xi;
        float wy = p.y - yi;

        float z0 = 0.0f;
        if( curr[ 0 ] > 0.0f ){
            if( curr[ 1 ] > _minDepth ){
                // both values valid
                z0 = Math::mix( curr[ 0 ], curr[ 1 ], wx );
            }  else {
                z0 = curr[ 0 ];
            }
        } else {
            if( curr[ 1 ] ){
                z0 = curr[ 1 ];
            } else {
                // no value valid in this line
                wy = 1.0f;
            }
        }

        float z1 = 0.0f;
        if( curr[ stride ] > 0.0f ){
            if( curr[ stride + 1 ] > 0.0f ){
                // both values valid
                z1 = Math::mix( curr[ stride ], curr[ stride + 1 ], wx );
            }  else {
                z1 = curr[ stride ];
            }
        } else {
            if( curr[ stride + 1 ] > 0.0f ){
                z1 = curr[ stride + 1 ];
            } else {
                // no value valid in this line
                wy = 0.0f;
            }
        }

        return Math::mix( z0, z1, wy ) * _depthScaling;
    }

    template <class WarpFunc, class Weighter>
    inline void  RGBDKeyframe<WarpFunc, Weighter>::initializePointLookUps( float* vals, size_t n, float foc, float c ) const
    {
        float invF = 1.0f / foc;
        for( size_t i = 0; i < n; i++ ){
            vals[ i ] = ( i - c ) * invF;
        }
    }
}

#endif // RGBDKEYFRAME_H
