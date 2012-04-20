#include <RGBDAlignment.h>
#include <cvt/vision/Vision.h>
#include <cvt/gfx/IMapScoped.h>
#include <cvt/util/EigenBridge.h>

namespace cvt
{
    RGBDAlignment::RGBDAlignment( const Matrix3f& K, size_t maxIterations, float depthScale ) :
        _intrinsics( K ),
        _maxIterations( maxIterations ),
        _depthScale( 1.0f / depthScale )
    {
    }

    RGBDAlignment::~RGBDAlignment()
    {
    }

    void RGBDAlignment::alignWithKeyframe( SE3<float>& predicted,
                                           const VOKeyframe& keyframe,
                                           const Image& gray )
    {
        size_t iter = 0;
        SIMD* simd = SIMD::instance();
        Matrix4f projMat;

        std::vector<Vector2f> warpedPts;
        warpedPts.resize( keyframe.numPoints() );

        Eigen::Matrix4f mEigen;
        mEigen.setIdentity();

        Eigen::Matrix3f Keigen;
        EigenBridge::toEigen( Keigen, _intrinsics );


        // sum of jacobians * delta
        VOKeyframe::JacType deltaSum;

        IMapScoped<const float> grayMap( gray );
        const float* kfPixels = keyframe.pixelData();
        const VOKeyframe::JacType* jacobians = keyframe.jacobians();
        const VOKeyframe::HessianType& invHess = keyframe.inverseHessian();

        size_t floatStride = grayMap.stride() / sizeof( float );

        while( iter < _maxIterations ){
            // build the updated projection Matrix
            const Eigen::Matrix4f& m = predicted.transformation();
            mEigen.block<3, 3>( 0, 0 ) = Keigen * m.block<3, 3>( 0, 0 );
            mEigen.block<3, 1>( 0, 3 ) = Keigen * m.block<3, 1>( 0, 3 );
            EigenBridge::toCVT( projMat, mEigen );

            // project the points:
            simd->projectPoints( &warpedPts[ 0 ], projMat, keyframe.pointsPtr(), keyframe.numPoints() );

            deltaSum.setZero();
            size_t numGood = 0;
            float ssd = 0.0f;

            for( size_t i = 0; i < warpedPts.size(); i++ ){
                const Vector2f& pw = warpedPts[ i ];
                if( pw.x > 0.0f && pw.x < ( gray.width()  - 1 ) &&
                    pw.y > 0.0f && pw.y < ( gray.height() - 1 ) ){
                    int lx = ( int )pw.x;
                    int ly = ( int )pw.y;
                    float fx = pw.x - lx;
                    float fy = pw.y - ly;

                    const float* p0 = grayMap.ptr() + ly * floatStride + lx;
                    const float* p1 = p0 + floatStride;

                    float v0 = Math::mix( p0[ 0 ], p0[ 1 ], fx );
                    float v1 = Math::mix( p1[ 0 ], p1[ 1 ], fx );
                    float v = Math::mix( v0, v1, fy );

                    // compute the delta
                    float delta = kfPixels[ i ] - v;
                    ssd += Math::sqr( delta );

                    VOKeyframe::JacType jtmp = delta * jacobians[ i ];

                    deltaSum += jtmp;
                    numGood++;
                }
            }

            // evaluate the delta parameters
            SE3<float>::ParameterVectorType deltaP = -invHess * deltaSum.transpose();
            predicted.applyInverse( -deltaP );

            iter++;
        }
    }
}
