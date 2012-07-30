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

namespace cvt
{
    template <class WarpFunc,
              class Weighter = NoWeighting<typename WarpFunc::Type> >
    class RGBDKeyframe {
        public:
            struct Result {
                Result() :
                    success( false ),
                    iterations( 0 ),
                    numPixels( 0 ),
                    costs( 0.0f )
                {
                }

                bool        success;
                size_t      iterations;
                size_t      numPixels;
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
                std::vector<PointType>      points3d;
                std::vector<float>          pixelValues;
                std::vector<JacobianType>   jacobians;
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

            RGBDKeyframe( const Mat3Type &K, size_t octaves, T scale );

            ~RGBDKeyframe();

            void updateOfflineData( const Matrix4<T>& pose, const ImagePyramid& pyramid, const Image& depth );

            const AlignmentData& dataForScale( size_t o ) const { return _dataForScale[ o ]; }
            const Matrix4<T>&    pose()                   const { return _pose; }

            void setDepthMapScaleFactor( float scaleFactor )        { _depthScaling = ( float )0xffff / scaleFactor; }
            void setMinimumDepth( T depthTresh )                    { _minDepth = depthTresh; }
            void setGradientThreshold( float thresh )               { _gradientThreshold = Math::sqr( thresh ); }
            void setMaxIter( size_t maxiter )                       { _maxIters = maxiter; }
            void setMinUpdate( T minUpdate )                        { _minUpdate = minUpdate; }
            void setTranslationJumpThreshold( T maxTDiff )          { _translationJumpThreshold = maxTDiff; }
            void setMinPixelPercentage( float minPixelPercentage )  { _minPixelPercentage = minPixelPercentage; }
            void setRobustParam( T v )                              { _weighter = Weighter( v ); }

            /**
             *  \brief align the current camera frame with this keyframe
             *  \param prediction   predicted pose of the camera frame in world frame (T_wc)
             */
            void align( Result& result, const Matrix4<T>& prediction, const ImagePyramid& pyr );

        private:

            Matrix4<T>                  _pose;
            std::vector<AlignmentData>  _dataForScale;
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

            void updateIntrinsics( const Mat3Type& K, T scale );
            void computeImageGradients( Image& gx, Image& gy, const Image& gray ) const;

            void alignSingleScale( Result& result, const Image& gray, const AlignmentData& kfdata );
            bool checkResult( const Result& res, const Matrix4<T> &lastPose, size_t numPixels ) const;

            void alignSingleScaleNonRobust( Result& result, const Image& gray, const AlignmentData& kfdata );
            void alignSingleScaleRobust( Result& result, const Image& gray, const AlignmentData& kfdata );

    };

    template <class WarpFunc, class Weighter>
    inline RGBDKeyframe<WarpFunc, Weighter>::RGBDKeyframe( const Mat3Type &K, size_t octaves, T scale ) :
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
        _minPixelPercentage( 0.3f ),
        _weighter( (T)0.1 )
    {
        _kx.scale( -0.5 );
        _ky.scale( -0.5 );

        _dataForScale.resize( octaves );
        updateIntrinsics( K, scale );
    }

    template <class WarpFunc, class Weighter>
    inline RGBDKeyframe<WarpFunc, Weighter>::~RGBDKeyframe()
    {
    }

    template <class WarpFunc, class Weighter>
    inline void RGBDKeyframe<WarpFunc, Weighter>::updateIntrinsics( const Mat3Type& K, T scale )
    {
        _dataForScale[ 0 ].intrinsics = K;
        for( size_t o = 1; o < _dataForScale.size(); o++ ){
            _dataForScale[ o ].intrinsics = _dataForScale[ o - 1 ].intrinsics * scale;
            _dataForScale[ o ].intrinsics[ 2 ][ 2 ] = ( T )1.0;
        }
    }

    template <class WarpFunc, class Weighter>
    inline void RGBDKeyframe<WarpFunc, Weighter>::updateOfflineData( const Matrix4<T>& poseMat,
                                                                                    const ImagePyramid& pyramid,
                                                                                    const Image& depth )
    {
        _pose = poseMat;

        Image gxI, gyI;
        float scale = 1.0f;

        Vector3<T> p3d;
        Eigen::Matrix<T, 2, 1> g;
        JacobianType j;
        HessianType H;

        IMapScoped<const float> depthMap( depth );

        for( size_t i = 0; i < pyramid.octaves(); i++ ){
            const Image& gray = pyramid[ i ];
            computeImageGradients( gxI, gyI, gray );

            AlignmentData& data = _dataForScale[ i ];
            data.clear();
            data.reserve( 0.6f * gray.width() * gray.height() );

            float invFx = 1.0f / data.intrinsics[ 0 ][ 0 ];
            float invFy = 1.0f / data.intrinsics[ 1 ][ 1 ];
            float cx    = data.intrinsics[ 0 ][ 2 ];
            float cy    = data.intrinsics[ 1 ][ 2 ];

            // temp vals
            std::vector<float> tmpx( gray.width() );
            std::vector<float> tmpy( gray.height() );

            for( size_t i = 0; i < tmpx.size(); i++ ){
                tmpx[ i ] = ( i - cx ) * invFx;
            }
            for( size_t i = 0; i < tmpy.size(); i++ ){
                tmpy[ i ] = ( i - cy ) * invFy;
            }

            IMapScoped<const float> gxMap( gxI );
            IMapScoped<const float> gyMap( gyI );
            IMapScoped<const float> grayMap( gray );

            H.setZero();
            for( size_t y = 0; y < gray.height(); y++ ){
                const float* gx = gxMap.ptr();
                const float* gy = gyMap.ptr();
                const float* value = grayMap.ptr();

                depthMap.setLine( scale * y );
                const float* d = depthMap.ptr();
                for( size_t x = 0; x < gray.width(); x++ ){
                    float z = d[ ( size_t ) scale * x ] * _depthScaling;
                    if( z > _minDepth ){
                        g[ 0 ] = gx[ x ];
                        g[ 1 ] = gy[ x ];

                        if( g.squaredNorm() < _gradientThreshold )
                            continue;

                        p3d[ 0 ] = tmpx[ x ] * z;
                        p3d[ 1 ] = tmpy[ y ] * z;
                        p3d[ 2 ] = z;

                        WarpFunc::computeJacobian( j, p3d, data.intrinsics, g, value[ x ] );

                        data.jacobians.push_back( j );
                        data.pixelValues.push_back( value[ x ] );
                        data.points3d.push_back( p3d );
                        H.noalias() += j.transpose() * j;
                    }
                }
                gxMap++;
                gyMap++;
                grayMap++;
            }

            // precompute the inverse hessian
            data.inverseHessian = H.inverse();

            scale /= pyramid.scaleFactor();
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
        //gray.convolve( gx, kx );
        //gray.convolve( gy, ky );
    }

    template <class WarpFunc, class Weighter>
    inline void RGBDKeyframe<WarpFunc, Weighter>::align( Result& result,
                                                         const Matrix4<T>& prediction,
                                                         const ImagePyramid& pyr )
    {
        Result scaleResult;

        Matrix4<T> tmp4;
        tmp4 = prediction.inverse() * _pose;

        result.warp.setPose( tmp4 );
        result.costs = 0.0f;
        result.iterations = 0;
        result.numPixels = 0;

        scaleResult = result;
        for( int o = pyr.octaves() - 1; o >= 0; o-- ){
            const AlignmentData& data = _dataForScale[ o ];
            alignSingleScale( scaleResult, pyr[ o ], data );

            if( checkResult( scaleResult, result.warp.poseMatrix(), data.points3d.size() ) ){
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
    inline void RGBDKeyframe<WarpFunc, Weighter>::alignSingleScale( Result& result, const Image& gray, const AlignmentData& kfdata )
    {
        if( IsRobustWeighting<Weighter>::Value ){
            alignSingleScaleRobust( result, gray, kfdata );
        } else {
            alignSingleScaleNonRobust( result, gray, kfdata );
        }
    }

    template <class WarpFunc, class Weighter>
    inline void RGBDKeyframe<WarpFunc, Weighter>::alignSingleScaleRobust( Result& result, const Image& gray, const AlignmentData& kfdata )
    {
        SIMD* simd = SIMD::instance();

        const size_t num = kfdata.points3d.size();
        const size_t width = gray.width();
        const size_t height = gray.height();

        std::vector<Vector2<T> > warpedPts;
        warpedPts.resize( num );

        std::vector<float> interpolatedPixels;
        interpolatedPixels.resize( num );

        Matrix4f projMat;
        Matrix4<T> K4( kfdata.intrinsics );

        // sum of jacobians * delta
        JacobianType deltaSum, jtmp;
        HessianType  hessian;

        IMapScoped<const float> grayMap( gray );

        result.iterations = 0;
        while( result.iterations < _maxIters ){
            // build the updated projection Matrix
            projMat = K4 * result.warp.poseMatrix();

            // project the points:
            simd->projectPoints( &warpedPts[ 0 ], projMat, &kfdata.points3d[ 0 ], kfdata.points3d.size() );
            simd->warpBilinear1f( &interpolatedPixels[ 0 ], &warpedPts[ 0 ].x, grayMap.ptr(), grayMap.stride(), width, height, -1.0f, num );

            deltaSum.setZero();
            hessian.setZero();

            result.numPixels = 0;
            result.costs = 0.0f;
            for( size_t i = 0; i < num; i++ ){
                if( interpolatedPixels[ i ] >= 0.0f ){
                    // compute the delta
                    float delta = result.warp.computeResidual( kfdata.pixelValues[ i ], interpolatedPixels[ i ] );
                    result.costs += Math::sqr( delta );
                    result.numPixels++;

                    T weight = _weighter.weight( delta );
                    jtmp = weight * kfdata.jacobians[ i ];

                    hessian.noalias() += jtmp.transpose() * kfdata.jacobians[ i ];
                    deltaSum += jtmp * delta;
                }
            }

            if( !result.numPixels ){
                break;
            }

            // evaluate the delta parameters
            typename WarpFunc::DeltaVectorType deltaP = -hessian.inverse() * deltaSum.transpose();
            result.warp.updateParameters( deltaP );

            result.iterations++;
            if( deltaP.norm() < _minUpdate )
                break;
        }
    }

    template <class WarpFunc, class Weighter>
    inline void RGBDKeyframe<WarpFunc, Weighter>::alignSingleScaleNonRobust( Result& result, const Image& gray, const AlignmentData& kfdata )
    {
        SIMD* simd = SIMD::instance();
        Matrix4f projMat;

        const size_t num = kfdata.points3d.size();
        const size_t width = gray.width();
        const size_t height = gray.height();
        std::vector<Vector2<T> > warpedPts;
        warpedPts.resize( num );

        std::vector<float> interpolatedPixels;
        interpolatedPixels.resize( num );

        Matrix4<T> K4( kfdata.intrinsics );

        // sum of jacobians * delta
        JacobianType deltaSum, jtmp;

        IMapScoped<const float> grayMap( gray );

        result.iterations = 0;
        while( result.iterations < _maxIters ){
            // build the updated projection Matrix
            projMat = K4 * result.warp.poseMatrix();

            // project the points:
            simd->projectPoints( &warpedPts[ 0 ], projMat, &kfdata.points3d[ 0 ], num );
            simd->warpBilinear1f( &interpolatedPixels[ 0 ], &warpedPts[ 0 ].x, grayMap.ptr(), grayMap.stride(), width, height, -1.0f, num );

            deltaSum.setZero();
            result.numPixels = 0;
            result.costs = 0.0f;
            for( size_t i = 0; i < num; i++ ){
                // compute the delta
                if( interpolatedPixels[ i ] >= 0.0f ){
                    float delta = result.warp.computeResidual( kfdata.pixelValues[ i ], interpolatedPixels[ i ] );
                    result.costs += Math::sqr( delta );
                    result.numPixels++;

                    jtmp = delta * kfdata.jacobians[ i ];
                    deltaSum += jtmp;
                }

            }

            if( !result.numPixels ){
                break;
            }

            // evaluate the delta parameters
            typename WarpFunc::DeltaVectorType deltaP = -kfdata.inverseHessian * deltaSum.transpose();
            result.warp.updateParameters( deltaP );

            result.iterations++;
            if( deltaP.norm() < _minUpdate )
                break;
        }
    }

    template <class WarpFunc, class Weighter>
    inline bool RGBDKeyframe<WarpFunc, Weighter>::checkResult( const Result& res, const Matrix4<T>& lastPose, size_t numPixels ) const
    {
        // to few pixels projected into image
        float pixelPercentage = (float)res.numPixels / ( float )numPixels;
        if( pixelPercentage < _minPixelPercentage ){
            return false;
        }

        // jump
        Matrix4<T> mat = res.warp.poseMatrix();
        if( ( mat.col( 3 ) - lastPose.col( 3 ) ).length() > _translationJumpThreshold ){
            return false;
        }
        return true;
    }
}

#endif // RGBDKEYFRAME_H
