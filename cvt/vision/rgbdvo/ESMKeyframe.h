/*
            CVT - Computer Vision Tools Library

     Copyright (c) 2012, Philipp Heise, Sebastian Klose

    THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
    KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
    IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
    PARTICULAR PURPOSE.
*/

#ifndef CVT_ESMKEYFRAME_H
#define CVT_ESMKEYFRAME_H

#include <cvt/math/Matrix.h>
#include <cvt/vision/ImagePyramid.h>
#include <cvt/gfx/IMapScoped.h>
#include <cvt/util/EigenBridge.h>
#include <cvt/vision/rgbdvo/RGBDKeyframe.h>
#include <Eigen/StdVector>

namespace cvt
{
    template <class WarpFunc>
    class ESMKeyframe : public RGBDKeyframe<WarpFunc> {
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

            ESMKeyframe( const Matrix3f &K, size_t octaves, float scale );
            ~ESMKeyframe();

			void updateOnlineData( const ImagePyramid& pyrf, const Image& depth );
            
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
    inline ESMKeyframe<WarpFunc>::ESMKeyframe( const Matrix3f &K, size_t octaves, float scale ) :
        RGBDKeyframe<WarpFunc>( K, octaves, scale ),
		_onlineGradientsX( octaves, scale ),
		_onlineGradientsY( octaves, scale )
    {
    }

    template <class WarpFunc>
    inline ESMKeyframe<WarpFunc>::~ESMKeyframe()
    {
    }

    template <class WarpFunc>
    inline void ESMKeyframe<WarpFunc>::recompute( std::vector<float>& residuals,
                                                 JacobianVec& jacobians,
                                                 const WarpFunc& warp,
                                                 const IMapScoped<const float>& gray,
                                                 size_t octave )
    {
        SIMD* simd = SIMD::instance();
        const size_t width = gray.width();
        const size_t height = gray.height();
        std::vector<Vector2f> warpedPts;
        std::vector<float> interpolatedPixels;
        std::vector<float> intGradX;
        std::vector<float> intGradY;

        const AlignmentData& data = this->dataForScale( octave );
        size_t n = data.size();

        // construct the projection matrix
        Matrix4f projMat( data.intrinsics() );
        projMat *= warp.pose();

        // resize the data storage
        warpedPts.resize( n );
        interpolatedPixels.resize( n );
        intGradX.resize( n );
        intGradY.resize( n );
        residuals.resize( n );
        jacobians.reserve( n );

        // project the points:
        simd->projectPoints( &warpedPts[ 0 ], projMat, &data.points()[ 0 ], n );

        // interpolate the pixel values
        simd->warpBilinear1f( &interpolatedPixels[ 0 ], &warpedPts[ 0 ].x, gray.ptr(), gray.stride(), width, height, -10.0f, n );

		// interpolate the gradients for ESM
		interpolateGradients( intGradX, _onlineGradientsX[ octave ], warpedPts, simd );
		interpolateGradients( intGradY, _onlineGradientsY[ octave ], warpedPts, simd );
        
		// compute the residuals
        warp.computeResiduals( &residuals[ 0 ], &data.pixels()[ 0 ], &interpolatedPixels[ 0 ], n );

        // sort out bad pixels (out of image)
        const JacobianVec& refJacs = data.jacobians();
        const ScreenJacVec& sj = data.screenJacobians();
        GradientType grad;
        size_t savePos = 0;
        // TODO: move this right after the bilinear interpolation step!
        for( size_t i = 0; i < n; ++i ){
            if( interpolatedPixels[ i ] >= 0.0f ){
                grad( 0, 0 ) = intGradX[ i ];
                grad( 0, 1 ) = intGradY[ i ];
                // compute the ESM jacobians
                jacobians[ savePos ] = 0.5f * ( refJacs[ i ] + grad * sj[ i ] );
                residuals[ savePos ] = residuals[ i ];
                ++savePos;
            }
        }
        residuals.erase( residuals.begin() + savePos, residuals.end() );
    }

	template <class WarpFunc>
    inline void ESMKeyframe<WarpFunc>::updateOnlineData( const ImagePyramid& pyrf, const Image& depth )
	{
		pyrf.convolve( _onlineGradientsX, this->_kx );
		pyrf.convolve( _onlineGradientsY, this->_ky );
	}

    template <class WarpFunc>
    inline void ESMKeyframe<WarpFunc>::interpolateGradients( std::vector<float>& result, const Image& gradImg, const std::vector<Vector2f>& positions, const SIMD* simd ) const
    {
        IMapScoped<const float> map( gradImg );
        simd->warpBilinear1f( &result[ 0 ], &positions[ 0 ].x, map.ptr(), map.stride(), gradImg.width(), gradImg.height(), -20.0f, positions.size() );
    }
}


#endif // ESMKEYFRAME_H
