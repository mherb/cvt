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

    struct VOParams
    {
        VOParams() :
            maxIters( 10 ),
            depthScale( 5000.0f ),
            gradientThreshold( 0.1f ),
            minDepth( 0.05f ),
            minParameterUpdate( 1e-5 )
        {}

        size_t  maxIters;
        float   depthScale;
        float   gradientThreshold;
        float   minDepth;
        float   minParameterUpdate;
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
            VOResult computeRelativePose( SE3<float>& predicted,
                                          const Image& gray,
                                          const Matrix3f& intrinsics,
                                          const VOParams& params ) const
            {
                return ( ( Derived* )this )->computeRelativePose( predicted, gray, intrinsics, params );
            }

        protected:
            ~KeyframeBase()
            {
            }
    };

}

#endif // KEYFRAMEBASE_H
