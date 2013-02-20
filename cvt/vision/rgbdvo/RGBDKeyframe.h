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
#include <cvt/vision/ImagePyramid.h>
#include <cvt/gfx/IMapScoped.h>
#include <cvt/util/EigenBridge.h>

#include <cvt/vision/rgbdvo/RGBDWarp.h>
#include <cvt/vision/rgbdvo/InformationSelection.h>

#include <Eigen/StdVector>

namespace cvt
{
    struct ScaleFeatures {
        std::vector<Vector2f>   positions;
        std::vector<size_t>     nonDepthFeatures;
    };

    // TODO: ALignmentData should be subclassed for ESM / IC / Fwd
    template <size_t dim>
    class AlignmentData {
        public:
            EIGEN_MAKE_ALIGNED_OPERATOR_NEW
            typedef Eigen::Matrix<float, 1, dim>    JacobianType;
            typedef Eigen::Matrix<float, 2, dim>    ScreenJacobianType;
            typedef std::vector<ScreenJacobianType, Eigen::aligned_allocator<ScreenJacobianType> > ScreenJacVec;
            typedef Eigen::Matrix<float, dim, dim>  HessianType;

            std::vector<Vector3f>       points3d;
            std::vector<float>          pixelValues;
            std::vector<JacobianType>   jacobians;

            /* reference screen jacobians of the warp*/
            ScreenJacVec                screenJacobians;
            std::vector<Vector2f>       gradients;

            HessianType                 hessian;
            HessianType                 inverseHessian;
            Matrix3f                    intrinsics;

            Image                       gray;
            Image                       gradX;
            Image                       gradY;

            void reserve( size_t size )
            {
                points3d.reserve( size );
                pixelValues.reserve( size );
                jacobians.reserve( size );

                screenJacobians.reserve( size );
                gradients.reserve( size );
            }

            void clear()
            {
                points3d.clear();
                pixelValues.clear();
                jacobians.clear();

                screenJacobians.clear();
                gradients.clear();

                hessian.setZero();
                inverseHessian.setZero();
            }
    };

    template <class Warp>
    class RGBDKeyframe {
        public:
            EIGEN_MAKE_ALIGNED_OPERATOR_NEW
            struct Result {
                EIGEN_MAKE_ALIGNED_OPERATOR_NEW
                Result() :
                    success( false ),
                    numPixels( 0 ),
                    pixelPercentage( 0.0f ),
                    costs( 0.0f )
                {
                }

                bool                success;
                size_t              iterations;
                size_t              numPixels;
                float               pixelPercentage; /* between 0 and 1 */
                float               costs;

                Warp                warp;
            };

            typedef Warp                            WarpType;
            typedef typename Warp::JacobianType     JacobianType;
            typedef typename Warp::HessianType      HessianType;
            typedef AlignmentData<Warp::NParams>    AlignDataType;

            RGBDKeyframe( const Matrix3f& K, size_t octaves, float scale );

            virtual ~RGBDKeyframe();

            const AlignDataType&    dataForScale( size_t o )       const { return _dataForScale[ o ]; }
            const Matrix4f&         pose()                         const { return _pose; }

            void updateOfflineData( const Matrix4f& pose, const ImagePyramid& pyramid, const Image& depth );

            // TODO: how can we handle this more nicely: the problem is, that the Image has type float and is normalized between 0.0f-1.0f
            // for uint32_t the max value is 0xFFFF, we want to convert to meters, therefore we need to define the scaling
            //  val pixvals = 1m -> scale by (float_denorm) * 1/val
            void setDepthMapScaleFactor( float val )                { _depthScaling = ( float )( 0xFFFF ) / val; }
            void setMinimumDepth( float depthTresh )                { _minDepth = depthTresh; }
            void setGradientThreshold( float thresh )               { _gradientThreshold = thresh; }
            void setTranslationJumpThreshold( float maxTDiff )      { _translationJumpThreshold = maxTDiff; }
            void setMinPixelPercentage( float minPixelPercentage )  { _minPixelPercentage = minPixelPercentage; }
            void setSelectionPixelPercentage( float n )             { _pixelPercentageToSelect = n; }

            void addPoints( const std::vector<Vector3f>& pts );

            virtual void updateOfflineDataForScale( AlignDataType& data,
                                                    const Image& gray,
                                                    const Image& depth,
                                                    float scale ) = 0;

            virtual void addPointsOnScale( AlignDataType& data,
                                           const std::vector<Vector3f>& pts,
                                           const Matrix4f& referenceToWorld ) = 0;

            /**
             * @brief kernelDx - retrieve the kernel used to compute x derivatives
             * @return kernelx
             */
            const IKernel& kernelDx() const { return _kx; }

            /**
             * @brief kernelDy - retrieve the kernel used to compute the y derivatives
             * @return kernely
             */
            const IKernel& kernelDy() const { return _ky; }

        protected:
            typedef std::vector<AlignDataType, Eigen::aligned_allocator<AlignDataType> > AlignmentDataVector;
            Matrix4f            _pose;
            AlignmentDataVector _dataForScale;
            Image               _depth;
            IKernel             _kx;
            IKernel             _ky;
            IKernel             _gaussX;
            IKernel             _gaussY;

            float               _depthScaling;
            float               _minDepth;
            float               _gradientThreshold;
            float               _translationJumpThreshold;
            float               _minPixelPercentage;
            float               _pixelPercentageToSelect;

            void updateIntrinsics( const Matrix3f& K, float scale );
            void computeImageGradients( Image& gx, Image& gy, const Image& gray ) const;

            bool checkResult( const Result& res, const Matrix4f& lastPose ) const;

            float interpolateDepth( const Vector2f& p, const float* ptr, size_t stride ) const;
            void  initializePointLookUps( float* vals, size_t n, float foc, float center ) const;
            void  selectInformation( AlignDataType& data, size_t n );
            void  updateHessian( AlignDataType& data );

//            /**
//             * @brief createDataForScale - Factory for the concrete AlignmentData for this reference
//             * @return
//             */
//            AlignDataType*  createDataForScale() = 0;
    };

    template <class Warp>
    inline RGBDKeyframe<Warp>::RGBDKeyframe( const Matrix3f& K, size_t octaves, float scale ) :
        //_kx( IKernel::HAAR_HORIZONTAL_3 ),
        //_ky( IKernel::HAAR_VERTICAL_3 ),
        _kx( IKernel::FIVEPOINT_DERIVATIVE_HORIZONTAL ),
        _ky( IKernel::FIVEPOINT_DERIVATIVE_VERTICAL ),
        //_gaussX( IKernel::GAUSS_HORIZONTAL_3 ),
        //_gaussY( IKernel::GAUSS_VERTICAL_3 ),
        _depthScaling( 1.0f ),
        _minDepth( 0.05 ),
        _gradientThreshold( 0.0f ),
        _minPixelPercentage( 0.2f ),
        _pixelPercentageToSelect( 0.3f )
    {
        //float s = -0.5f;
        //float s = -2.0f;
        float s = -1.0f;
        _kx.scale( s );
        _ky.scale( s );

        _dataForScale.resize( octaves );
        updateIntrinsics( K, scale );
    }

    template <class WarpFunc>
    inline RGBDKeyframe<WarpFunc>::~RGBDKeyframe()
    {
    }

    template <class WarpFunc>
    inline void RGBDKeyframe<WarpFunc>::updateOfflineData( const Matrix4f& poseMat,
                                                           const ImagePyramid& pyramid,
                                                           const Image& depth )
    {
        this->_pose = poseMat;

        float scale = 1.0f;
        for( size_t i = 0; i < pyramid.octaves(); i++ ){
            this->updateOfflineDataForScale( _dataForScale[ i ], pyramid[ i ], depth, scale );
            scale /= pyramid.scaleFactor();
        }
    }

    template <class WarpFunc>
    inline void RGBDKeyframe<WarpFunc>::addPoints( const std::vector<Vector3f>& pts )
    {
        Matrix4f refToWorld = _pose.inverse();
        for( size_t i = 0; i < _dataForScale.size(); i++ ){
            this->addPointsOnScale( _dataForScale[ i ], pts, refToWorld );
        }
    }

    template <class WarpFunc>
    inline void RGBDKeyframe<WarpFunc>::updateIntrinsics( const Matrix3f& K, float scale )
    {
        _dataForScale[ 0 ].intrinsics = K;
        for( size_t o = 1; o < _dataForScale.size(); ++o ){
            _dataForScale[ o ].intrinsics = _dataForScale[ o - 1 ].intrinsics * scale;
            _dataForScale[ o ].intrinsics[ 2 ][ 2 ] = 1.0f;
        }
    }

    template <class WarpFunc>
    inline void RGBDKeyframe<WarpFunc>::computeImageGradients( Image& gx, Image& gy, const Image& gray ) const
    {
        gx.reallocate( gray.width(), gray.height(), IFormat::GRAY_FLOAT );
        gy.reallocate( gray.width(), gray.height(), IFormat::GRAY_FLOAT );

        // sobel style
        //gray.convolve( gx, _kx, _gaussY );
        //gray.convolve( gy, _gaussX, _ky );

        // normal
        gray.convolve( gx, _kx );
        gray.convolve( gy, _ky );
    }

    template <class WarpFunc>
    inline bool RGBDKeyframe<WarpFunc>::checkResult( const Result& res, const Matrix4f& lastPose ) const
    {
        // to few pixels projected into image
        if( res.pixelPercentage < _minPixelPercentage ){
            //std::cout << "Pixel Percentage: " << res.pixelPercentage << " : " << _minPixelPercentage << std::endl;
            return false;
        }

        // jump
        Matrix4f mat = res.warp.poseMatrix();
//        if( ( mat.col( 3 ) - lastPose.col( 3 ) ).length() > _translationJumpThreshold ){
//            //std::cout << "Delta T: " << mat.col( 3 ) << " : " << lastPose.col( 3 ) << std::endl;
//            return false;
//        }

        return true;
    }

    template <class WarpFunc>
    inline float RGBDKeyframe<WarpFunc>::interpolateDepth( const Vector2f& p, const float* ptr, size_t stride ) const
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

    template <class WarpFunc>
    inline void  RGBDKeyframe<WarpFunc>::initializePointLookUps( float* vals, size_t n, float foc, float c ) const
    {
        float invF = 1.0f / foc;
        for( size_t i = 0; i < n; i++ ){
            vals[ i ] = ( i - c ) * invF;
        }
    }

    template <class WarpFunc>
    inline void RGBDKeyframe<WarpFunc>::selectInformation( AlignDataType &data, size_t n )
    {
        InformationSelection<JacobianType> selector( n );
        const std::set<size_t>& ids = selector.selectInformation( &data.jacobians[ 0 ], data.jacobians.size() );

        // now rearrange the data according to the ids:
        std::set<size_t>::const_iterator it = ids.begin();
        const std::set<size_t>::const_iterator end = ids.end();

        size_t saveIdx = 0;
        data.hessian.setZero();
        while( it != end ){
            data.jacobians[ saveIdx ] = data.jacobians[ *it ];
            data.points3d[ saveIdx ] = data.points3d[ *it ];
            data.pixelValues[ saveIdx ] = data.pixelValues[ *it ];
            data.hessian.noalias() += data.jacobians[ saveIdx ].transpose() * data.jacobians[ saveIdx ];

            saveIdx++;
            it++;
        }

        // remove the rest
        data.jacobians.erase( data.jacobians.begin() + n, data.jacobians.end() );
        data.points3d.erase( data.points3d.begin() + n, data.points3d.end() );
        data.pixelValues.erase( data.pixelValues.begin() + n, data.pixelValues.end() );
    }

    template <class WarpFunc>
    inline void RGBDKeyframe<WarpFunc>::updateHessian( AlignDataType& data )
    {
        typename std::vector<JacobianType>::const_iterator it = data.jacobians.begin();
        typename std::vector<JacobianType>::const_iterator end = data.jacobians.end();
        data.hessian.setZero();
        while( it != end ){
            data.hessian.noalias() += it->transpose() * *it;
            it++;
        }
    }
}

#endif // RGBDKEYFRAME_H
