/*
            CVT - Computer Vision Tools Library

     Copyright (c) 2012, Philipp Heise, Sebastian Klose

    THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
    KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
    IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
    PARTICULAR PURPOSE.
*/
#ifndef CVT_GLOBALFC_KEYFRAME_H
#define CVT_GLOBALFC_KEYFRAME_H

#include <cvt/vision/rgbdvo/RGBDKeyframe.h>

namespace cvt {
    template <class WarpFunc>
    class GlobalFCKeyframe : public RGBDKeyframe<WarpFunc> {
        public:
            EIGEN_MAKE_ALIGNED_OPERATOR_NEW
            typedef RGBDKeyframe<WarpFunc>              Base;
            typedef float                               T;
            typedef typename Base::JacobianType         JacobianType;
            typedef typename Base::ScreenJacobianType   ScreenJacobianType;
            typedef typename Base::JacobianVec          JacobianVec;
            typedef typename Base::ScreenJacVec         ScreenJacVec;
            typedef typename Base::AlignmentData        AlignmentData;
            typedef typename Base::GradientType         GradientType;

            GlobalFCKeyframe( const Matrix3f &K, size_t octaves, float scale );
            ~GlobalFCKeyframe();

            void updateOnlineData( const ImagePyramid& pyrf, const Image& depth );

            virtual void updateOfflineDataForScale( AlignmentData& data,
                                                    const Image& gray,
                                                    const Image& depth,
                                                    float scale );

            void recompute( std::vector<float>& residuals,
                            JacobianVec& jacobians,
                            const WarpFunc& warp,
                            const IMapScoped<const float>& gray,
                            size_t octave );
        private:
            ImagePyramid	_onlineGradientsX;
            ImagePyramid	_onlineGradientsY;

            void interpolateGradients( std::vector<float>& result, const Image& gradImg, const std::vector<Vector2f>& positions, const SIMD* simd ) const;
    };

    template <class WarpFunc>
    inline GlobalFCKeyframe<WarpFunc>::GlobalFCKeyframe( const Matrix3f &K, size_t octaves, float scale ) :
        RGBDKeyframe<WarpFunc>( K, octaves, scale ),
        _onlineGradientsX( octaves, scale ),
        _onlineGradientsY( octaves, scale )
    {
    }

    template <class WarpFunc>
    inline GlobalFCKeyframe<WarpFunc>::~GlobalFCKeyframe()
    {
    }

    template <class WarpFunc>
    inline void GlobalFCKeyframe<WarpFunc>::recompute( std::vector<float>& residuals,
                                                       JacobianVec& jacobians,
                                                       const WarpFunc& warp,
                                                       const IMapScoped<const float>& gray,
                                                       size_t octave )
    {
        SIMD* simd = SIMD::instance();
        const size_t width = gray.width();
        const size_t height = gray.height();
        std::vector<Vector2f> warpedPts;
        std::vector<Vector3f> ptsInCam;
        std::vector<float> interpolatedPixels;
        std::vector<float> intGradX;
        std::vector<float> intGradY;

        const AlignmentData& data = this->dataForScale( octave );
        size_t n = data.size();

        // construct the projection matrix
        Matrix4f projMat( data.intrinsics() );
        Matrix4f poseMat = warp.pose();

        // resize the data storage
        warpedPts.resize( n );
        ptsInCam.resize( n );
        interpolatedPixels.resize( n );
        intGradX.resize( n );
        intGradY.resize( n );
        residuals.resize( n );
        jacobians.resize( n );

        // transform points to camera coordinate frame
        simd->transformPoints( &ptsInCam[ 0 ], poseMat, &data.points()[ 0 ], n );

        // project the points:
        simd->projectPoints( &warpedPts[ 0 ], projMat, &ptsInCam[ 0 ], n );

        // interpolate the pixel values
        simd->warpBilinear1f( &interpolatedPixels[ 0 ], &warpedPts[ 0 ].x, gray.ptr(), gray.stride(), width, height, -10.0f, n );

		// evaluate the gradients at the warped positions
		interpolateGradients( intGradX, _onlineGradientsX[ octave ], warpedPts, simd );
		interpolateGradients( intGradY, _onlineGradientsY[ octave ], warpedPts, simd );

        // compute the residuals
        warp.computeResiduals( &residuals[ 0 ], &data.pixels()[ 0 ], &interpolatedPixels[ 0 ], n );

        // sort out bad pixels (out of image)        
        GradientType grad;
        size_t savePos = 0;

        for( size_t i = 0; i < n; ++i ){
            if( interpolatedPixels[ i ] >= 0.0f ){
                grad.coeffRef( 0, 0 ) = intGradX[ i ];
                grad.coeffRef( 0, 1 ) = intGradY[ i ];

                // compute the Fwd jacobians
                WarpFunc::computeJacobian( jacobians[ savePos ], ptsInCam[ i ], data.intrinsics(), grad, interpolatedPixels[ i ] );
                residuals[ savePos ] = residuals[ i ];
                ++savePos;
            }
        }
        residuals.erase( residuals.begin() + savePos, residuals.end() );
        jacobians.erase( jacobians.begin() + savePos, jacobians.end() );
    }

	template <class WarpFunc>
    inline void GlobalFCKeyframe<WarpFunc>::updateOnlineData( const ImagePyramid& pyrf, const Image& /*depth*/ )
	{
		pyrf.convolve( _onlineGradientsX, this->_kx );
		pyrf.convolve( _onlineGradientsY, this->_ky );        
	}

    template <class WarpFunc>
    inline void GlobalFCKeyframe<WarpFunc>::interpolateGradients( std::vector<float>& result, const Image& gradImg, const std::vector<Vector2f>& positions, const SIMD* simd ) const
    {
        IMapScoped<const float> map( gradImg );
        simd->warpBilinear1f( &result[ 0 ], &positions[ 0 ].x, map.ptr(), map.stride(), gradImg.width(), gradImg.height(), -20.0f, positions.size() );
    }

    template <class WarpFunc>
    inline void GlobalFCKeyframe<WarpFunc>::updateOfflineDataForScale( AlignmentData& data,
                                                                       const Image& gray,
                                                                       const Image& depth,
                                                                       float scale )
    {
        IMapScoped<const float> depthMap( depth );
        const float* d = depthMap.ptr();
        size_t depthStride = depthMap.stride() / sizeof( float );

        Vector2f currP;
        Vector3f p3d, p3dw;
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
        this->initializePointLookUps( &tmpx[ 0 ], tmpx.size(), intr[ 0 ][ 0 ], intr[ 0 ][ 2 ] );
        this->initializePointLookUps( &tmpy[ 0 ], tmpy.size(), intr[ 1 ][ 1 ], intr[ 1 ][ 2 ] );

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
                float z = this->interpolateDepth( currP, d, depthStride );
                if( z > this->_minDepth && z < this->_maxDepth ){
                    g( 0, 0 ) = gx[ x ];
                    g( 0, 1 ) = gy[ x ];

                    float salience = Math::abs( g.coeff( 0, 0 ) ) + Math::abs( g.coeff( 0, 1 ) );
                    if( salience < this->_gradientThreshold )
                        continue;

                    p3d[ 0 ] = tmpx[ x ] * z;
                    p3d[ 1 ] = tmpy[ y ] * z;
                    p3d[ 2 ] = z;
                    p3dw = this->_pose * p3d;

                    WarpFunc::screenJacobian( sj, p3d, data.intrinsics() );
                    data.add( p3dw, sj, g, value[ x ] );
                }
            }
            gxMap++;
            gyMap++;
            grayMap++;
        }
    }



}

#endif // CVT_GlobalFCKeyframe_H
