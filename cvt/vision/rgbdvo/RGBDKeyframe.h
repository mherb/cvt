
#ifndef RGBDKEYFRAME_H
#define RGBDKEYFRAME_H

#include <cvt/math/Matrix.h>
#include <cvt/vision/ImagePyramid.h>
#include <cvt/gfx/IMapScoped.h>
#include <cvt/util/EigenBridge.h>

#include <cvt/vision/rgbdvo/RGBDWarp.h>
#include <cvt/vision/rgbdvo/InformationSelection.h>
#include <cvt/vision/rgbdvo/Linearizer.h>

#include <Eigen/StdVector>

namespace cvt
{
    template <class AlignData>
    class RGBDKeyframe {
        public:
            EIGEN_MAKE_ALIGNED_OPERATOR_NEW
			typedef AlignData							AlignDataType;
			typedef typename AlignDataType::WarpType	WarpType;
			typedef typename WarpType::JacobianType     JacobianType;
			typedef typename WarpType::ScreenJacType    ScreenJacobianType;
			typedef typename WarpType::HessianType      HessianType;
			typedef Eigen::Matrix<float, 1, 2>			GradientType;

            typedef std::vector<ScreenJacobianType, Eigen::aligned_allocator<ScreenJacobianType> > ScreenJacVec;
            typedef std::vector<JacobianType, Eigen::aligned_allocator<JacobianType> > JacobianVec;           

            RGBDKeyframe( const Matrix3f& K, size_t octaves, float scale );

            virtual ~RGBDKeyframe();

			const AlignData&  dataForScale( size_t o )	const { return _dataForScale[ o ]; }
			const Matrix4f&   pose()					const { return _pose; }

            // TODO: handle this more nicely: the problem is, that the Image has type float and is normalized between 0.0f-1.0f
            // for uint32_t the max value is 0xFFFF, we want to convert to meters, therefore we need to define the scaling
            // #val pixvals corresponds to 1m --> scale by (uint16_t max) * 1/val
            void setDepthMapScaleFactor( float val )        { _depthScaling = ( float )( 0xFFFF ) / val; }
            void setMinimumDepth( float depthTresh )        { _minDepth = depthTresh; }
            void setMaximumDepth( float depthTresh )        { _maxDepth = depthTresh; }
            void setGradientThreshold( float thresh )       { _gradientThreshold = thresh; }
            void setSelectionPixelPercentage( float n )     { _pixelPercentageToSelect = n; }
            void setUseInformationSelection( bool v )       { _useInformationSelection = v; }

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

			virtual void updateOfflineData( const Matrix4f& pose, const ImagePyramid& pyramid, const Image& depth );

			virtual void updateOfflineDataForScale( AlignData& data,
                                                    const Image& gray,
                                                    const Image& depth,
                                                    float scale ) = 0;

            size_t dataSize( size_t octave ) const { return _dataForScale[ octave ].size(); }

			virtual void updateOnlineData( const ImagePyramid& /*pyrf*/, const Image& /*depth*/ ){}

            virtual void recompute( std::vector<float>& residuals,
                                    JacobianVec& jacobians,
									const WarpType& warp,
                                    const IMapScoped<const float>& gray,
                                    const IMapScoped<const float>& depth,
                                    size_t octave ) = 0;

            /**
             * \brief update the input/measurement of the costfunction
             * \ref input pyramid of the current intensity image, depth is the current depth image
             */
            virtual void updateInput( const ImagePyramid& gray, const Image& depth ){ /* TODO */ }


        protected:
            typedef std::vector<AlignData, Eigen::aligned_allocator<AlignData> > AlignmentDataVector;
            Matrix4f            _pose;
            AlignmentDataVector _dataForScale;
            Image               _depth;
            IKernel             _kx;
            IKernel             _ky;
            IKernel             _gaussX;
            IKernel             _gaussY;

            float               _depthScaling;
            float               _minDepth;
            float               _maxDepth;
            float               _gradientThreshold;
            float               _pixelPercentageToSelect;
            bool                _useInformationSelection;

            void updateIntrinsics( const Matrix3f& K, float scale );
            void computeImageGradients( Image& gx, Image& gy, const Image& gray ) const;

            float interpolateDepth( const Vector2f& p, const float* ptr, size_t stride ) const;
            void  initializePointLookUps( float* vals, size_t n, float foc, float center ) const;
    };

	template <class AlignData>
	inline RGBDKeyframe<AlignData>::RGBDKeyframe( const Matrix3f& K, size_t octaves, float scale ) :
		_kx( IKernel::FIVEPOINT_DERIVATIVE_HORIZONTAL ),
		_ky( IKernel::FIVEPOINT_DERIVATIVE_VERTICAL ),
        _depthScaling( 1.0f ),
        _minDepth( 0.05 ),
        _maxDepth( 10.0 ),
        _gradientThreshold( 0.0f ),
        _pixelPercentageToSelect( 0.3f ),
        _useInformationSelection( false )
    {
        float s = -1.0f;
        _kx.scale( s );
        _ky.scale( s );

        _dataForScale.resize( octaves );
        updateIntrinsics( K, scale );
    }

	template <class AlignData>
	inline RGBDKeyframe<AlignData>::~RGBDKeyframe()
    {
    }

	template <class AlignData>
	inline void RGBDKeyframe<AlignData>::updateOfflineData( const Matrix4f& poseMat,
															const ImagePyramid& pyramid,
															const Image& depth )
    {
        this->_pose = poseMat;

        float scale = 1.0f;
        size_t nPixels = pyramid[ 0 ].width() * pyramid[ 0 ].height();
        for( size_t i = 0; i < pyramid.octaves(); i++ ){
            this->updateOfflineDataForScale( _dataForScale[ i ], pyramid[ i ], depth, scale );
            if( _useInformationSelection ){
                _dataForScale[ i ].selectInformation( nPixels * Math::sqr( scale ) * _pixelPercentageToSelect );
            }
            scale /= pyramid.scaleFactor();
        }
    }

	template <class AlignData>
	inline void RGBDKeyframe<AlignData>::updateIntrinsics( const Matrix3f& K, float scale )
    {
        _dataForScale[ 0 ].setIntrinsics( K );
        Matrix3f Ks = K;
        for( size_t o = 1; o < _dataForScale.size(); ++o ){
            Ks *= scale;
            Ks[ 2 ][ 2 ] = 1.0f;
            _dataForScale[ o ].setIntrinsics( Ks );
        }
    }

	template <class AlignData>
	inline void RGBDKeyframe<AlignData>::computeImageGradients( Image& gx, Image& gy, const Image& img ) const
    {
        gx.reallocate( img.width(), img.height(), IFormat::GRAY_FLOAT );
        gy.reallocate( img.width(), img.height(), IFormat::GRAY_FLOAT );

        // sobel style
        //gray.convolve( gx, _kx, _gaussY );
        //gray.convolve( gy, _gaussX, _ky );

        // normal
        img.convolve( gx, _kx );
        img.convolve( gy, _ky );
    }

	template <class AlignData>
	inline float RGBDKeyframe<AlignData>::interpolateDepth( const Vector2f& p, const float* ptr, size_t stride ) const
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

	template <class AlignData>
	inline void  RGBDKeyframe<AlignData>::initializePointLookUps( float* vals, size_t n, float foc, float c ) const
    {
        float invF = 1.0f / foc;
        for( size_t i = 0; i < n; i++ ){
            vals[ i ] = ( i - c ) * invF;
        }
    }

}

#endif // RGBDKEYFRAME_H
