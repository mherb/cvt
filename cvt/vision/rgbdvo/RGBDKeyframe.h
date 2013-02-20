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

    // TODO: ALignmentData should be subclassed for ESM / IC / Fwd


    template <class Warp>
    class RGBDKeyframe {
        public:
            EIGEN_MAKE_ALIGNED_OPERATOR_NEW
            typedef Warp                            WarpType;
            typedef typename Warp::JacobianType     JacobianType;
            typedef typename Warp::ScreenJacType    ScreenJacobianType;
            typedef typename Warp::HessianType      HessianType;
            typedef Eigen::Matrix<float, 1, 2>      GradientType;
            typedef std::vector<ScreenJacobianType, Eigen::aligned_allocator<ScreenJacobianType> > ScreenJacVec;
            typedef std::vector<JacobianType, Eigen::aligned_allocator<JacobianType> > JacobianVec;

            class AlignmentData {
                public:
                    Image                       gray;
                    Image                       gradX;
                    Image                       gradY;

                    EIGEN_MAKE_ALIGNED_OPERATOR_NEW
                    void reserve( size_t size )
                    {
                        _points3d.reserve( size );
                        _pixelValues.reserve( size );
                        _jacobians.reserve( size );
                        _screenJacobians.reserve( size );
                    }
                    void clear()
                    {
                        _points3d.clear();
                        _pixelValues.clear();
                        _jacobians.clear();
                        _screenJacobians.clear();
                    }

                    void add( const Vector3f& point,
                              const ScreenJacobianType& jac,
                              const GradientType& iGrad,
                              float val )
                    {
                        _points3d.push_back( point );
                        _screenJacobians.push_back( jac );
                        _jacobians.push_back( iGrad * jac );
                        _pixelValues.push_back( val );
                    }

                    size_t size() const { return _points3d.size(); }

                    const Matrix3f& intrinsics() const { return _intrinsics; }
                    void setIntrinsics( const Matrix3f& intr ){ _intrinsics = intr; }

                    const std::vector<Vector3f>&     points()    const { return _points3d; }
                    const std::vector<float>&        pixels()    const { return _pixelValues; }

                    // these are the offline jacobians
                    const JacobianVec& jacobians() const { return _jacobians; }

                protected:
                    std::vector<Vector3f>       _points3d;
                    std::vector<float>          _pixelValues;
                    JacobianVec                 _jacobians;
                    ScreenJacVec                _screenJacobians;
                    Matrix3f                    _intrinsics;
            };

            RGBDKeyframe( const Matrix3f& K, size_t octaves, float scale );

            virtual ~RGBDKeyframe();

            const AlignmentData&    dataForScale( size_t o )       const { return _dataForScale[ o ]; }
            const Matrix4f&         pose()                         const { return _pose; }

            void updateOfflineData( const Matrix4f& pose, const ImagePyramid& pyramid, const Image& depth );

            // TODO: how can we handle this more nicely: the problem is, that the Image has type float and is normalized between 0.0f-1.0f
            // for uint32_t the max value is 0xFFFF, we want to convert to meters, therefore we need to define the scaling
            //  val pixvals = 1m -> scale by (float_denorm) * 1/val
            void setDepthMapScaleFactor( float val )                { _depthScaling = ( float )( 0xFFFF ) / val; }
            void setMinimumDepth( float depthTresh )                { _minDepth = depthTresh; }
            void setGradientThreshold( float thresh )               { _gradientThreshold = thresh; }
            void setSelectionPixelPercentage( float n )             { _pixelPercentageToSelect = n; }

            void addPoints( const std::vector<Vector3f>& pts );

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

            virtual void updateOfflineDataForScale( AlignmentData& data,
                                                    const Image& gray,
                                                    const Image& depth,
                                                    float scale );

            size_t dataSize( size_t octave ) const { return _dataForScale[ octave ].size(); }

            virtual void updateOnlineData( const ImagePyramid& /*pyrf*/, const Image& /*depth*/ ){}

            virtual void recompute( std::vector<float>& residuals,
                                    JacobianVec& jacobians,
                                    const Warp& warp,
                                    const IMapScoped<const float>& gray,
                                    size_t octave ) = 0;


        protected:
            typedef std::vector<AlignmentData, Eigen::aligned_allocator<AlignmentData> > AlignmentDataVector;
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
            float               _pixelPercentageToSelect;

            void updateIntrinsics( const Matrix3f& K, float scale );
            void computeImageGradients( Image& gx, Image& gy, const Image& gray ) const;

            float interpolateDepth( const Vector2f& p, const float* ptr, size_t stride ) const;
            void  initializePointLookUps( float* vals, size_t n, float foc, float center ) const;
            void  selectInformation( AlignmentData& data, size_t n );
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
        _dataForScale[ 0 ].setIntrinsics( K );
        Matrix3f Ks = K;
        for( size_t o = 1; o < _dataForScale.size(); ++o ){
            Ks *= scale;
            Ks[ 2 ][ 2 ] = 1.0f;
            _dataForScale[ o ].setIntrinsics( Ks );
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
    inline void RGBDKeyframe<WarpFunc>::selectInformation( AlignmentData &data, size_t n )
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

            saveIdx++;
            it++;
        }

        // remove the rest
        data.jacobians.erase( data.jacobians.begin() + n, data.jacobians.end() );
        data.points3d.erase( data.points3d.begin() + n, data.points3d.end() );
        data.pixelValues.erase( data.pixelValues.begin() + n, data.pixelValues.end() );
    }

    template <class WarpFunc>
    inline void RGBDKeyframe<WarpFunc>::updateOfflineDataForScale( AlignmentData& data,
                                                                   const Image& gray,
                                                                   const Image& depth,
                                                                   float scale )
    {
        IMapScoped<const float> depthMap( depth );
        const float* d = depthMap.ptr();
        size_t depthStride = depthMap.stride() / sizeof( float );

        Vector2f currP;
        Vector3f p3d;
        GradientType g;
        JacobianType j;
        ScreenJacobianType sj;

        // compute the image gradients
        this->computeImageGradients( data.gradX, data.gradY, gray );
        data.gray = gray;

        data.clear();
        size_t pixelsOnOctave = ( gray.width() - 1 ) * ( gray.height() - 1 );
        data.reserve( 0.4f * pixelsOnOctave );

        // TODO: replace this by a simd function!
        // temp vals
        std::vector<float> tmpx( gray.width() );
        std::vector<float> tmpy( gray.height() );

        const Matrix3f& intr = data.intrinsics();
        initializePointLookUps( &tmpx[ 0 ], tmpx.size(), intr[ 0 ][ 0 ], intr[ 0 ][ 2 ] );
        initializePointLookUps( &tmpy[ 0 ], tmpy.size(), intr[ 1 ][ 1 ], intr[ 1 ][ 2 ] );

        IMapScoped<const float> gxMap( data.gradX );
        IMapScoped<const float> gyMap( data.gradY );
        IMapScoped<const float> grayMap( data.gray );

        for( size_t y = 0; y < gray.height() - 1; y++ ){
            const float* gx = gxMap.ptr();
            const float* gy = gyMap.ptr();
            const float* value = grayMap.ptr();

            // scale the point
            currP.y = scale * y;

            for( size_t x = 0; x < gray.width() - 1; x++ ){
                currP.x = scale * x;
                float z = interpolateDepth( currP, d, depthStride );
                if( z > this->_minDepth && z < 10.0f ){
                    g( 0, 0 ) = gx[ x ];
                    g( 0, 1 ) = gy[ x ];

                    float salience = Math::abs( g( 0, 0 ) ) + Math::abs( g( 0, 1 ) );
                    if( salience < _gradientThreshold )
                        continue;

                    p3d[ 0 ] = tmpx[ x ] * z;
                    p3d[ 1 ] = tmpy[ y ] * z;
                    p3d[ 2 ] = z;

                    WarpFunc::screenJacobian( sj, p3d, data.intrinsics() );
                    //WarpFunc::computeJacobian( j, p3d, data.intrinsics, g, value[ x ] );

                    data.add( p3d, sj, g, value[ x ] );
                }
            }
            gxMap++;
            gyMap++;
            grayMap++;
        }
        // TODO: run informationselector on the data!!
    }

}

#endif // RGBDKEYFRAME_H
