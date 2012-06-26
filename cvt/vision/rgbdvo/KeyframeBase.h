/*
            CVT - Computer Vision Tools Library

     Copyright (c) 2012, Philipp Heise, Sebastian Klose

    THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
    KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
    IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
    PARTICULAR PURPOSE.
*/

#ifndef CVT_KEYFRAMEBASE_H
#define CVT_KEYFRAMEBASE_H

#include <cvt/gfx/Image.h>
#include <cvt/math/Matrix.h>
#include <cvt/math/SE3.h>

namespace cvt {

    struct VOResult {
        float  SSD;
        size_t numPixels;
        size_t iterations;
    };

    struct PoseRepresentation
    {
        PoseRepresentation() :
            gain( 0.0f ), bias( 0.0f )
        {
        }

        SE3<float>  pose;
        float       gain;
        float       bias;
    };

    struct VOParams
    {
        VOParams() :
            maxIters( 10 ),
            depthScale( 5000.0f ),
            gradientThreshold( 0.1f ),
            minDepth( 0.05f ),
            minParameterUpdate( 1e-5 ),
            robustParam( 0.1f ),
            octaves( 3 ),
            pyrScale( 0.5f )
        {}

        size_t  maxIters;
        float   depthScale;
        float   gradientThreshold;
        float   minDepth;
        float   minParameterUpdate;
        float	robustParam;

        // for the multiscale
        size_t  octaves;
        float	pyrScale;
    };

    template <class Derived>
    class KeyframeBase
    {
        public:

            /**
             * \param	gray            gray Image (float)
             * \param	depth           Depth Image (float)
             * \param	pose            pose for this keyframe
             * \param   K               the intrinsics for the rgb image
             * \param   dephtScaling    scale factor of the depth image: depthScaling equals to 1m!
             */
            static Derived* create( const Image& gray, const Image& depth, const Matrix4f& pose, const Matrix3f& K, const VOParams& params )
            {
                // derived HAS to have this constructor
                return new Derived( gray, depth, pose, K, params );
            }

            const Matrix4f& pose() const { return ( ( Derived* )this )->pose(); }

            /**
             *  \brief copmute the relative pose of an image w.r.t. this keyframe
             *  \param  predicted   input/output the pose of the image w.r.t. this keyframe
             *  \param  gray        the grayscale image of type GRAY_FLOAT
             *  \return Result information (ssd, iterations, numPixel, ...)
             */
            VOResult computeRelativePose( PoseRepresentation& predicted,
                                          const Image& gray,
                                          const Matrix3f& intrinsics,
                                          const VOParams& params )
            {
                return ( ( Derived* )this )->computeRelativePose( predicted, gray, intrinsics, params );
            }

        protected:
            ~KeyframeBase()
            {
            }

            float interpolatePixelValue( const Vector2f& pos, const float* ptr, size_t stride ) const;
            void computeGradients( Image& gx, Image& gy, const Image gray ) const;
    };

    template <class Derived>
    inline float KeyframeBase<Derived>::interpolatePixelValue( const Vector2f& pos, const float* ptr, size_t stride ) const
    {
        int lx = ( int )pos.x;
        int ly = ( int )pos.y;
        float fx = pos.x - lx;
        float fy = pos.y - ly;

        const float* p0 = ptr + ly * stride + lx;
        const float* p1 = p0 + stride;

        float v0 = Math::mix( p0[ 0 ], p0[ 1 ], fx );
        float v1 = Math::mix( p1[ 0 ], p1[ 1 ], fx );
        return Math::mix( v0, v1, fy );
    }

    template <class Derived>
    inline void KeyframeBase<Derived>::computeGradients( Image& gx, Image& gy, const Image gray ) const
    {
        IKernel kx = IKernel::HAAR_HORIZONTAL_3;
        IKernel ky = IKernel::HAAR_VERTICAL_3;
        kx.scale( -0.5f );
        ky.scale( -0.5f );

        gx.reallocate( gray.width(), gray.height(), IFormat::GRAY_FLOAT );
        gy.reallocate( gray.width(), gray.height(), IFormat::GRAY_FLOAT );

        gray.convolve( gx, kx );
        gray.convolve( gy, ky );
    }

}

#endif // KEYFRAMEBASE_H
