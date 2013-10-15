#ifndef CVT_INTENSITYKEYFRAME_H
#define CVT_INTENSITYKEYFRAME_H

#include <cvt/vision/rgbdvo/RGBDKeyframe.h>

namespace cvt {
	template <class AlignData, class LinearizerType>
	class IntensityKeyframe : public RGBDKeyframe<AlignData> {
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

            IntensityKeyframe( const Matrix3f &K, size_t octaves, float scale );
            ~IntensityKeyframe();

            void updateOfflineDataForScale( AlignData &data,
                                            const Image& gray,
                                            const Image& depth,
                                            float scale );

            void updateOnlineData( const ImagePyramid& pyrf,
                                   const Image& depth );

            void recompute( std::vector<float>& residuals,
                            JacobianVec& jacobians,
							const typename Base::WarpType& warp,
                            const IMapScoped<const float>& gray,
                            const IMapScoped<const float>& depth,
                            size_t octave );
        protected:
            LinearizerType  _linearizer;
    };

	template <class AlignData, class LinearizerType>
	inline IntensityKeyframe<AlignData, LinearizerType>::IntensityKeyframe( const Matrix3f &K, size_t octaves, float scale ) :
		RGBDKeyframe<AlignData>( K, octaves, scale ),
		_linearizer( this->_kx, this->_ky, octaves, scale )
    {
    }

	template <class AlignData, class LinearizerType>
	inline IntensityKeyframe<AlignData, LinearizerType>::~IntensityKeyframe()
    {
    }

    template <class AlignData, class LinearizerType>
    inline void IntensityKeyframe<AlignData, LinearizerType>::updateOfflineDataForScale( AlignData &data,
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

                    // point in world coord frame
                    p3dw = this->_pose * p3d;

                    WarpType::screenJacobian( sj, p3d, data.intrinsics() );

                    data.add( p3dw, sj, g, value[ x ] );
                }
            }
            gxMap++;
            gyMap++;
            grayMap++;
        }
    }

	template <class AlignData, class LinearizerType>
	inline void IntensityKeyframe<AlignData, LinearizerType>::recompute( std::vector<float>& residuals,
																		 JacobianVec& jacobians,
																		 const typename Base::WarpType &warp,
																		 const IMapScoped<const float>& gray,
																		 const IMapScoped<const float>& /*depth*/,
																		 size_t octave )
    {
        SIMD* simd = SIMD::instance();
        const size_t width = gray.width();
        const size_t height = gray.height();
        std::vector<Vector2f> warpedPts;
        std::vector<float> interpolatedPixels;

		const AlignData& data = this->dataForScale( octave );
        size_t n = data.size();

        // construct the projection matrix
        Matrix4f projMat( data.intrinsics() );
        projMat *= warp.pose();

        // resize the data storage
        warpedPts.resize( n );
        interpolatedPixels.resize( n );
        residuals.resize( n );
        jacobians.resize( n );

        // project the points:
        simd->projectPoints( &warpedPts[ 0 ], projMat, &data.points()[ 0 ], n );

        // interpolate the pixel values
        simd->warpBilinear1f( &interpolatedPixels[ 0 ], &warpedPts[ 0 ].x, gray.ptr(), gray.stride(), width, height, -10.0f, n );

        // compute the residuals
        warp.computeResiduals( &residuals[ 0 ], &data.pixels()[ 0 ], &interpolatedPixels[ 0 ], n );

        _linearizer.recomputeJacobians( jacobians, residuals, warpedPts, interpolatedPixels, data, octave );
    }

	template <class WarpFunc, class LinearizerType>
	inline void IntensityKeyframe<WarpFunc, LinearizerType>::updateOnlineData( const ImagePyramid& pyrf,
																			   const Image& depth )
	{
		_linearizer.updateOnlineData( pyrf, depth );
	}

}

#endif
