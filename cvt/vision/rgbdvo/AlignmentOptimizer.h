/*
            CVT - Computer Vision Tools Library
            
     Copyright (c) 2012, Philipp Heise, Sebastian Klose
     
    THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
    KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
    IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
    PARTICULAR PURPOSE.
*/

#ifndef CVT_ALIGNMENTOPTIMIZER_H
#define CVT_ALIGNMENTOPTIMIZER_H

#include <cvt/vision/rgbdvo/RGBDKeyframe.h>

namespace cvt
{
    template <class T>
    class AlignmentOptimizer {
        public:
            struct Result {
                size_t      iterations;
                size_t      numPixels;
                float       costs;
                Matrix4<T>  pose;
            };

            AlignmentOptimizer( size_t maxIters, T minUpdate );
            ~AlignmentOptimizer();

            void align( Result& res, const Matrix4<T>& prediction, const ImagePyramid& pyr, const RGBDKeyframe<T>& keyframe );

            void setMaxIter( size_t maxiter ) { _maxIters = maxiter; }
            void setMinUpdate( T minUpdate )  { _minUpdate = minUpdate; }
            void setTranslationJumpThreshold( T maxTDiff )  { _translationJumpThreshold = maxTDiff; }
            void setMinPixelPercentage( float minPixelPercentage )  { _minPixelPercentage = minPixelPercentage; }

        private:
            size_t  _maxIters;
            T       _minUpdate;
            T       _translationJumpThreshold;
            float   _minPixelPercentage;

            void alignSingleScale( Result& res, const Image& gray, const typename RGBDKeyframe<T>::AlignmentData& kfdata );

            float interpolatePixelValue( const Vector2f& pos, const float* ptr, size_t stride ) const;

            bool checkResult( const Result& res, const Matrix4<T> &lastPose, size_t numPixels ) const;

    };

    template <class T>
    inline AlignmentOptimizer<T>::AlignmentOptimizer( size_t maxIters, T minUpdate ) :
        _maxIters( maxIters ),
        _minUpdate( minUpdate ),
        _translationJumpThreshold( ( T )0.8 ),
        _minPixelPercentage( 0.3f )
    {
    }

    template <class T>
    inline AlignmentOptimizer<T>::~AlignmentOptimizer()
    {
    }

    template <class T>
    inline void AlignmentOptimizer<T>::align( Result& result,
                                       const Matrix4<T>& prediction,
                                       const ImagePyramid& pyr,
                                       const RGBDKeyframe<T>& keyframe )
    {
        Result scaleResult;

        result.pose = prediction.inverse();
        bool isRelative = ( keyframe.pointStorageType() == RGBDKeyframe<T>::STORE_RELATIVE );
        if( isRelative ){
            result.pose *= keyframe.pose();
        }

        scaleResult = result;
        for( int o = pyr.octaves() - 1; o >= 0; o-- ){
            alignSingleScale( scaleResult, pyr[ o ], keyframe.dataForScale( o ) );

            if( checkResult( scaleResult, result.pose, keyframe.dataForScale( o ).points3d.size() ) ){
                // seems to be a good alignment
                result = scaleResult;
            } else {
                scaleResult = result;
            }
        }

        result.pose.inverseSelf();
        if( isRelative ){
            result.pose = keyframe.pose() * result.pose;
        }
    }

    template <class T>
    inline void AlignmentOptimizer<T>::alignSingleScale( Result& result,
                                                         const Image& gray,
                                                         const typename RGBDKeyframe<T>::AlignmentData& kfdata )
    {
        SIMD* simd = SIMD::instance();
        Matrix4f projMat;

        std::vector<Vector2<T> > warpedPts;
        warpedPts.resize( kfdata.points3d.size() );

        Eigen::Matrix<T, 4, 4> mEigen( Eigen::Matrix<T, 4, 4>::Identity() );
        Eigen::Matrix<T, 3, 3> Keigen;
        EigenBridge::toEigen( Keigen, kfdata.intrinsics );

        // sum of jacobians * delta
        typename RGBDKeyframe<T>::JacobianType deltaSum, jtmp;

        IMapScoped<const float> grayMap( gray );
        size_t floatStride = grayMap.stride() / sizeof( float );

        Eigen::Matrix<T, 4, 4> m;
        EigenBridge::toEigen( m, result.pose );
        SE3<T> pose;
        pose.set( m );

        result.iterations = 0;
        while( result.iterations < _maxIters ){
            // build the updated projection Matrix
            mEigen.template block<3, 3>( 0, 0 ) = Keigen * pose.transformation().template block<3, 3>( 0, 0 );
            mEigen.template block<3, 1>( 0, 3 ) = Keigen * pose.transformation().template block<3, 1>( 0, 3 );
            EigenBridge::toCVT( projMat, mEigen );

            // project the points:
            simd->projectPoints( &warpedPts[ 0 ], projMat, &kfdata.points3d[ 0 ], kfdata.points3d.size() );

            deltaSum.setZero();
            result.numPixels = 0;
            result.costs = 0.0f;
            for( size_t i = 0; i < warpedPts.size(); i++ ){
                const Vector2<T> & pw = warpedPts[ i ];

                if( pw.x > 0.0f && pw.x < ( gray.width()  - 1 ) &&
                    pw.y > 0.0f && pw.y < ( gray.height() - 1 ) ){
                    float v = interpolatePixelValue( pw, grayMap.ptr(), floatStride );

                    // compute the delta
                    float delta = kfdata.pixelValues[ i ] - v;
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
            SE3<float>::ParameterVectorType deltaP = -kfdata.inverseHessian * deltaSum.transpose();
            pose.applyInverse( -deltaP );

            result.iterations++;
            if( deltaP.norm() < _minUpdate )
                break;
        }

        // convert the pose
        EigenBridge::toCVT( result.pose, pose.transformation() );
    }

    template <class T>
    inline float AlignmentOptimizer<T>::interpolatePixelValue( const Vector2f& pos, const float* ptr, size_t stride ) const
    {
        int   lx = ( int )pos.x;
        int   ly = ( int )pos.y;
        float fx = pos.x - lx;
        float fy = pos.y - ly;

        const float* p0 = ptr + ly * stride + lx;
        const float* p1 = p0 + stride;

        float v0 = Math::mix( p0[ 0 ], p0[ 1 ], fx );
        float v1 = Math::mix( p1[ 0 ], p1[ 1 ], fx );
        return Math::mix( v0, v1, fy );
    }

    template <class T>
    inline bool AlignmentOptimizer<T>::checkResult( const Result& res, const Matrix4<T>& lastPose, size_t numPixels ) const
    {
        // to few pixels projected into image
        float pixelPercentage = (float)res.numPixels / ( float )numPixels;
        if( pixelPercentage < _minPixelPercentage ){
            return false;
        }

        // jump
        if( ( res.pose.col( 3 ) - lastPose.col( 3 ) ).length() > _translationJumpThreshold ){
            return false;
        }

        return true;
    }
}

#endif // ALIGNMENTOPTIMIZER_H
