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

#include <cvt/vision/rgbdvo/RGBDKeyframe.h>

namespace cvt
{
	template <class AlignData>
	class ESMKeyframe : public RGBDKeyframe<AlignData> {
        public:
            EIGEN_MAKE_ALIGNED_OPERATOR_NEW
			typedef RGBDKeyframe<AlignData>             Base;
			typedef typename Base::WarpType				WarpType;
            typedef float                               T;
            typedef typename Base::JacobianType         JacobianType;
            typedef typename Base::ScreenJacobianType   ScreenJacobianType;
            typedef typename Base::JacobianVec          JacobianVec;
            typedef typename Base::ScreenJacVec         ScreenJacVec;
            typedef typename Base::GradientType         GradientType;

            ESMKeyframe( const Matrix3f &K, size_t octaves, float scale );
            ~ESMKeyframe();

            void updateOnlineData( const ImagePyramid& pyrf, const Image& depth );
            
			void recompute( std::vector<float>& residuals,
                            JacobianVec& jacobians,
							const WarpType& warp,
                            const IMapScoped<const float>& gray,
                            size_t octave );
		private:
			ImagePyramid	_onlineGradientsX;
			ImagePyramid	_onlineGradientsY;

            void interpolateGradients( std::vector<float>& result, const Image& gradImg, const std::vector<Vector2f>& positions, const SIMD* simd ) const;
    };

	template <class AlignData>
	inline ESMKeyframe<AlignData>::ESMKeyframe( const Matrix3f &K, size_t octaves, float scale ) :
		RGBDKeyframe<AlignData>( K, octaves, scale ),
		_onlineGradientsX( octaves, scale ),
		_onlineGradientsY( octaves, scale )
    {
    }

	template <class AlignData>
	inline ESMKeyframe<AlignData>::~ESMKeyframe()
    {
    }

	template <class AlignData>
	inline void ESMKeyframe<AlignData>::recompute( std::vector<float>& residuals,
                                                 JacobianVec& jacobians,
												 const WarpType& warp,
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

		const AlignData& data = this->dataForScale( octave );
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
        jacobians.resize( n );

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
        // TODO: move this right after the bilinear interpolation step?
        JacobianType jCur;
        for( size_t i = 0; i < n; ++i ){
            if( interpolatedPixels[ i ] >= 0.0f ){
                grad.coeffRef( 0, 0 ) = intGradX[ i ];
                grad.coeffRef( 0, 1 ) = intGradY[ i ];
                // compute the ESM jacobians
				WarpType::computeJacobian( jCur, sj[ i ], grad, interpolatedPixels[ i ] );
                jacobians[ savePos ] = 0.5f * ( refJacs[ i ] + jCur );
                residuals[ savePos ] = residuals[ i ];
                ++savePos;
            }
        }
        residuals.erase( residuals.begin() + savePos, residuals.end() );
        jacobians.erase( jacobians.begin() + savePos, jacobians.end() );
    }

	template <class AlignData>
	inline void ESMKeyframe<AlignData>::updateOnlineData( const ImagePyramid& pyrf, const Image& /*depth*/ )
	{
		pyrf.convolve( _onlineGradientsX, this->_kx );
		pyrf.convolve( _onlineGradientsY, this->_ky );
	}

	template <class AlignData>
	inline void ESMKeyframe<AlignData>::interpolateGradients( std::vector<float>& result, const Image& gradImg, const std::vector<Vector2f>& positions, const SIMD* simd ) const
    {
        IMapScoped<const float> map( gradImg );
        simd->warpBilinear1f( &result[ 0 ], &positions[ 0 ].x, map.ptr(), map.stride(), gradImg.width(), gradImg.height(), -20.0f, positions.size() );
    }
}


#endif // ESMKEYFRAME_H
