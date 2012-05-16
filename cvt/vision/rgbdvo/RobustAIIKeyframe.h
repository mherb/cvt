#ifndef CVT_ROBUST_AII_KEYFRAME_H
#define CVT_ROBUST_AII_KEYFRAME_H

#include <cvt/gfx/Image.h>
#include <cvt/math/Matrix.h>
#include <cvt/math/SE3.h>

#include <vector>
#include <Eigen/Core>
#include <cvt/vision/rgbdvo/KeyframeBase.h>
#include <cvt/gfx/IMapScoped.h>
#include <cvt/math/SE3.h>
#include <cvt/util/EigenBridge.h>

namespace cvt
{
	template <class WeighterType>
    class RobustAIIKeyframe : public KeyframeBase<RobustAIIKeyframe<WeighterType> >
    {
        public:			
            typedef Eigen::Matrix<float, 8, 8> HessianType;
            typedef Eigen::Matrix<float, 1, 8> JacType;

            /**
             * \param	gray            gray Image (float)
             * \param	depth           Depth Image (float)
             * \param	pose            pose for this keyframe
             * \param   K               the intrinsics for the rgb image
             * \param   params          parameters
             */
            RobustAIIKeyframe( const Image& gray, const Image& depth, const Matrix4f& pose, const Matrix3f& K, const VOParams& params );

            ~RobustAIIKeyframe();

            const Matrix4f&     pose()                  const { return _pose; }

            /**
             *  \brief copmute the relative pose of an image w.r.t. this keyframe
             *  \param  predicted   input/output the pose of the image w.r.t. this keyframe
             *  \param  gray        the grayscale image of type GRAY_FLOAT
             *  \return Result information (ssd, iterations, numPixel, ...)
             */
            VOResult computeRelativePose( PoseRepresentation& predicted,
                                          const Image& gray,
                                          const Matrix3f& intrinsics,
                                          const VOParams& params ) const;

			
        protected:
            Matrix4f                    _pose;
            Image                       _gray;

            // the 3D points of this keyframe
            std::vector<Vector3f>       _points3d;

            // the pixel values (gray) for the points
            std::vector<float>          _pixelValues;

            // jacobians for that points
            std::vector<JacType, Eigen::aligned_allocator<JacType> > _jacobians;

			WeighterType				_weighter;

            void computeJacobians( const Image& depth, const Matrix3f& intrinsics, const VOParams& params );
			using KeyframeBase<RobustAIIKeyframe<WeighterType> >::computeGradients;
			using KeyframeBase<RobustAIIKeyframe<WeighterType> >::interpolatePixelValue;
    };


	template <class Weighter>
    inline RobustAIIKeyframe<Weighter>::RobustAIIKeyframe( const Image& gray, const Image& depth,
													 const Matrix4f& pose, const Matrix3f& K, const VOParams& params ) :
        _pose( pose ),
        _gray( gray ),
		_weighter( params.robustParam )
    {
        computeJacobians( depth, K, params );
    }

	template <class Weighter>
    inline RobustAIIKeyframe<Weighter>::~RobustAIIKeyframe()
    {       	
    }

	template <class Weighter>
    inline void RobustAIIKeyframe<Weighter>::computeJacobians( const Image& depth, const Matrix3f& intrinsics, const VOParams& params )
    {
        Image gxI, gyI;
        computeGradients( gxI, gyI, _gray );

        float depthScaling = ( float )0xffff / params.depthScale;

        float invFx = 1.0f / intrinsics[ 0 ][ 0 ];
        float invFy = 1.0f / intrinsics[ 1 ][ 1 ];
        float cx    = intrinsics[ 0 ][ 2 ];
        float cy    = intrinsics[ 1 ][ 2 ];

        // temp vals
        std::vector<float> tmpx( depth.width() );
        std::vector<float> tmpy( depth.height() );

        for( size_t i = 0; i < tmpx.size(); i++ ){
            tmpx[ i ] = ( i - cx ) * invFx;
        }
        for( size_t i = 0; i < tmpy.size(); i++ ){
            tmpy[ i ] = ( i - cy ) * invFy;
        }

        IMapScoped<const float> gxMap( gxI );
        IMapScoped<const float> gyMap( gyI );
        IMapScoped<const float> grayMap( _gray );
        IMapScoped<const float> depthMap( depth );

        // eval the jacobians:
        Eigen::Vector3f p3d;
        Eigen::Vector2f g;
        JacType j;
        SE3<float>::ScreenJacType J;

        HessianType H( HessianType::Zero() );

        Eigen::Matrix3f K;
        EigenBridge::toEigen( K, intrinsics );
        SE3<float> pose;

        float gradThreshold = Math::sqr( params.gradientThreshold );

        for( size_t y = 0; y < depth.height(); y++ ){
            const float* gx = gxMap.ptr();
            const float* gy = gyMap.ptr();
            const float* value = grayMap.ptr();
            const float* d = depthMap.ptr();
            for( size_t x = 0; x < depth.width(); x++ ){
                float z = d[ x ] * depthScaling;
                if( z > params.minDepth ){
                    g[ 0 ] = gx[ x ];
                    g[ 1 ] = gy[ x ];

                    if( g.squaredNorm() < gradThreshold )
                        continue;

                    p3d[ 0 ] = tmpx[ x ] * z;
                    p3d[ 1 ] = tmpy[ y ] * z;
                    p3d[ 2 ] = z;

                    pose.screenJacobian( J, p3d, K );

                    j.head<6>() = g.transpose() * J;
					j[ 6 ] = value[ x ];
					j[ 7 ] = 1.0f;

                    _jacobians.push_back( j );
                    _pixelValues.push_back( value[ x ] );
                    _points3d.push_back( Vector3f( p3d[ 0 ], p3d[ 1 ], p3d[ 2 ] ) );                    
                    H.noalias() += j.transpose() * j;                    
                }
            }
            gxMap++;
            gyMap++;
            grayMap++;
            depthMap++;
        }
    }

	template <class Weighter>
    inline VOResult RobustAIIKeyframe<Weighter>::computeRelativePose( PoseRepresentation& predicted,
																  const Image& gray,
                                              			 		  const Matrix3f& intrinsics,
                                              			 		  const VOParams& params ) const
    {
        VOResult result;
        result.SSD = 0.0f;
        result.iterations = 0;
        result.numPixels = 0;

        SIMD* simd = SIMD::instance();
        Matrix4f projMat;

        std::vector<Vector2f> warpedPts;
        warpedPts.resize( _points3d.size() );

        Eigen::Matrix4f mEigen;
        mEigen.setIdentity();

        Eigen::Matrix3f Keigen;
        EigenBridge::toEigen( Keigen, intrinsics );

		HessianType H;

        // sum of jacobians * delta
        JacType deltaSum, jtmp;

        IMapScoped<const float> grayMap( gray );
        size_t floatStride = grayMap.stride() / sizeof( float );

        while( result.iterations < params.maxIters ){
            // build the updated projection Matrix
            const Eigen::Matrix4f& m = predicted.pose.transformation();
            mEigen.block<3, 3>( 0, 0 ) = Keigen * m.block<3, 3>( 0, 0 );
            mEigen.block<3, 1>( 0, 3 ) = Keigen * m.block<3, 1>( 0, 3 );
            EigenBridge::toCVT( projMat, mEigen );

            // project the points:
            simd->projectPoints( &warpedPts[ 0 ], projMat, &_points3d[ 0 ], _points3d.size() );

            deltaSum.setZero();
            result.numPixels = 0;
            result.SSD = 0.0f;

			H.setZero();

            for( size_t i = 0; i < warpedPts.size(); i++ ){
                const Vector2f& pw = warpedPts[ i ];
                if( pw.x > 0.0f && pw.x < ( gray.width()  - 1 ) &&
                    pw.y > 0.0f && pw.y < ( gray.height() - 1 ) ){
                    float v = interpolatePixelValue( pw, grayMap.ptr(), floatStride );
                    
					// bias gain:
                    v = ( 1.0f + predicted.gain ) * v + predicted.bias;

                    // compute the delta
                    float delta = _pixelValues[ i ] - v;
                    result.SSD += Math::sqr( delta );
                    result.numPixels++;
                    
					jtmp = _weighter.weight( delta ) * _jacobians[ i ];

					H += jtmp.transpose() * jtmp;
                    deltaSum += ( delta * jtmp );
                }
            }

            // evaluate the delta parameters
			Eigen::Matrix<float, 8, 1> deltaP = -H.inverse() * deltaSum.transpose();
            predicted.pose.applyInverse( -deltaP.head<6>() );
            // update bias and gain
            predicted.bias = ( predicted.bias - deltaP[ 7 ] ) / ( 1.0f + deltaP[ 6 ] );
            predicted.gain = ( predicted.gain - deltaP[ 6 ] ) / ( 1.0f + deltaP[ 6 ] );

            result.iterations++;
            if( deltaP.norm() < params.minParameterUpdate )
                return result;
        }
        return result;
    }
}

#endif
