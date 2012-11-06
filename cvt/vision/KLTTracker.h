/*
            CVT - Computer Vision Tools Library

     Copyright (c) 2012, Philipp Heise, Sebastian Klose

    THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
    KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
    IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
    PARTICULAR PURPOSE.
 */
#ifndef CVT_KLT_TRACKER_H
#define CVT_KLT_TRACKER_H

#include <cvt/math/Vector.h>
#include <cvt/geom/Line2D.h>
#include <cvt/gfx/Image.h>
#include <vector>

#include <cvt/vision/KLTPatch.h>
#include <cvt/math/Translation2D.h>
#include <cvt/math/Math.h>
#include <cvt/util/EigenBridge.h>
#include <cvt/util/CVTAssert.h>

namespace cvt
{
    template <class PoseType, size_t pSize=16>
        class KLTTracker
        {
            public:
                typedef KLTPatch<pSize, PoseType>	KLTPType;

                KLTTracker( size_t maxIters = 20 );

                /**
                 *	\brief	set the maximum number of iterations
                 */
                void setMaxIterations( size_t maxIter ) { _maxIters = maxIter; }

                /**
                 *	\brief	get the maximum number of iterations
                 */
                size_t maxIterations() const { return _maxIters; }

                /* track a single scale patch */
                bool trackPatch( KLTPType& patch,
                                 const float *current, size_t currStride,
                                 size_t width, size_t height );


                /* track patch using scale space */
                bool trackPatchMultiscale( KLTPType& patch,
                                          const ImagePyramid& pyramid );

            private:
                size_t _maxIters;

                /* warps all the patch pixels */
                float buildSystem( KLTPType& patch, typename KLTPType::JacType& jacSum,
                                   const Matrix3f& pose,
                                   const float *iPtr, size_t iStride,
                                   size_t width, size_t height,
                                   size_t octave = 0 );

                bool patchIsInImage( const Matrix3f& pose, size_t w, size_t h ) const;
        };


        template <class PoseType, size_t pSize>
        inline KLTTracker<PoseType, pSize>::KLTTracker( size_t maxIters ) :
            _maxIters( maxIters )
        {
        }

        template <class PoseType, size_t pSize>
        inline bool KLTTracker<PoseType, pSize>::trackPatch( KLTPType& patch,
                                                             const float* current, size_t currStride,
                                                             size_t width, size_t height )
        {
            typename KLTPType::JacType jSum;
            typename PoseType::ParameterVectorType delta;

            float diffSum = 0.0f;
            const float maxDiff = 0.7 * Math::sqr( pSize * 255.0f );

            size_t iter = 0;
            while( iter < _maxIters ){
                jSum.setZero();
                diffSum = 0.0f;

                //pose matrix
                Matrix3f pose;
                EigenBridge::toCVT( pose, patch.pose().transformation() );

                // first test if all points transform into the image
                if( !patchIsInImage( pose, width, height ) ){
                    return false;
                }

                diffSum = buildSystem( patch, jSum, pose, current, currStride, width, height );
                if( diffSum > maxDiff )
                    return false;

                // solve for the delta:
                delta = patch.inverseHessian() * jSum;
                patch.pose().applyInverse( -delta );

                iter++;
            }
            return true;
        }

        template <class PoseType, size_t pSize>
        bool KLTTracker<PoseType, pSize>::trackPatchMultiscale( KLTPType& patch,
                                                                const ImagePyramid& pyramid )
        {
            CVT_ASSERT( pyramid[ 0 ].format() == IFormat::GRAY_FLOAT, "Format must be GRAY_FLOAT!" );

            typename KLTPType::JacType jSum;
            typename PoseType::ParameterVectorType delta;

            size_t nOctaves = pyramid.octaves();
            float scale = Math::pow( pyramid.scaleFactor(), nOctaves-1 );
            float invScale = 1.0f / pyramid.scaleFactor();

            // get the pose of the patch
            Matrix3f poseMat;
            EigenBridge::toCVT( poseMat, patch.pose().transformation() );
            poseMat[ 0 ][ 2 ] *= scale;
            poseMat[ 1 ][ 2 ] *= scale;

            PoseType tmpPose;
            tmpPose.set( poseMat );

            const float maxDiff = Math::sqr( pSize * 255.0f );
            float diffSum = maxDiff;
            for( int oc = pyramid.octaves() - 1; oc >= 0; --oc ){
                IMapScoped<const float> map( pyramid[ oc ] );
                size_t w = pyramid[ oc ].width();
                size_t h = pyramid[ oc ].height();
                size_t iter = 0;
                diffSum = Math::sqr( pSize * 255 );
                while( iter < _maxIters ){
                    //pose matrix
                    EigenBridge::toCVT( poseMat, tmpPose.transformation() );

                    // first test if all points of the patch transform into the image
                    if( !patchIsInImage( poseMat, w, h ) ){
                        return false;
                    }

                    jSum.setZero();
                    diffSum = buildSystem( patch, jSum, poseMat,
                                           map.ptr(), map.stride(),
                                           w, h,
                                           oc );

                    if( diffSum >= maxDiff ){
                        return false;
                    }

                    // solve for the delta:
                    delta = patch.inverseHessian( oc ) * jSum;

                    if( delta.norm() < 1e-6 )
                        break;

                    tmpPose.applyInverse( -delta );

                    iter++;
                }

                if( oc != 0 ){
                    // we need to scale up
                    EigenBridge::toCVT( poseMat, tmpPose.transformation() );
                    poseMat[ 0 ][ 2 ] *= invScale;
                    poseMat[ 1 ][ 2 ] *= invScale;
                    tmpPose.set( poseMat );
                }
            }

            // set the final patch pose accordingly
            patch.pose().transformation() = tmpPose.transformation();
            return true;
        }

        template <class PoseType, size_t pSize>
        float KLTTracker<PoseType, pSize>::buildSystem( KLTPType& patch,
                                                        typename KLTPType::JacType& jacSum,
                                                        const Matrix3f& pose,
                                                        const float* imgPtr, size_t iStride,
                                                        size_t width, size_t height,
                                                        size_t octave )
        {
            // the warped current ones
            float* warped = patch.transformed( octave );
            // the original template pixels
            const float* temp = patch.pixels( octave );
            const typename KLTPType::JacType* J = patch.jacobians( octave );

            float diffSum = 0.0f;

            // check for nans in matrix
            for( size_t i = 0; i < 3; i++ )
                for( size_t k = 0; k < 3; k++ )
                    if( Math::isNaN( pose[ i ][ k ] ) )
                        return Math::MAXF;

            SIMD* simd = SIMD::instance();
            // transform the points:
            std::vector<Vector2f> warpedPts( KLTPType::numPatchPoints() );
            simd->transformPoints( &warpedPts[ 0 ], pose, KLTPType::patchPoints(), warpedPts.size() );
            simd->warpBilinear1f( warped, &warpedPts[ 0 ].x, imgPtr, iStride, width, height, -1.0f, warpedPts.size() );

            // compute the residuals
            float residuals[ pSize * pSize ];
            simd->Sub( residuals, warped, temp, warpedPts.size() );

            size_t num = warpedPts.size();
            const float* r = residuals;
            while( num-- ){
                diffSum += Math::sqr( *r );
                jacSum += ( *J *  *r );
                J++;
                r++;
            }
            return diffSum;
        }

        template <class PoseType, size_t pSize>
        bool KLTTracker<PoseType, pSize>::patchIsInImage( const Matrix3f& pose, size_t w, size_t h ) const
        {
            static const float half = pSize >> 1;
            static const Vector2f a( -half, -half );
            static const Vector2f b(  half, -half );
            static const Vector2f c(  half,  half );
            static const Vector2f d( -half,  half );

            Vector2f pWarped;

            pWarped = pose * a;
            if( pWarped.x < 0.0f || pWarped.x >= w ||
                pWarped.y < 0.0f || pWarped.y >= h )
                return false;

            pWarped = pose * b;
            if( pWarped.x < 0.0f || pWarped.x >= w ||
                pWarped.y < 0.0f || pWarped.y >= h )
                return false;

            pWarped = pose * c;
            if( pWarped.x < 0.0f || pWarped.x >= w ||
                pWarped.y < 0.0f || pWarped.y >= h )
                return false;

            pWarped = pose * d;
            if( pWarped.x < 0.0f || pWarped.x >= w ||
                pWarped.y < 0.0f || pWarped.y >= h )
                return false;
            return true;
        }

}

#endif
