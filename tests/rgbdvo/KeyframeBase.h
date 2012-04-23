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

    template <class Derived>
    class KeyframeBase
    {
        public:
            struct Result {
                float  SSD;
                size_t numPixels;
                size_t iterations;
            };

            /**
             * \param	gray            gray Image (float)
             * \param	depth           Depth Image (float)
             * \param	pose            pose for this keyframe
             * \param   K               the intrinsics for the rgb image
             * \param   dephtScaling    scale factor of the depth image: depthScaling equals to 1m!
             */
            static Derived* create( const Image& gray, const Image& depth, const Matrix4f& pose, const Matrix3f& K, float depthScaling )
            {
                // derived HAS to have this constructor
                return new Derived( gray, depth, pose, K, depthScaling );
            }

            ~KeyframeBase()
            {
                delete ( Derived* )this;
            }

            const Matrix4f& pose() const { return ( ( Derived* )this )->pose(); }

            /**
             *  \brief copmute the relative pose of an image w.r.t. this keyframe
             *  \param  predicted   input/output the pose of the image w.r.t. this keyframe
             *  \param  gray        the grayscale image of type GRAY_FLOAT
             *  \return Result information (ssd, iterations, numPixel, ...)
             */
            Result computeRelativePose( SE3<float>& predicted,
                                        const Image& gray,
                                        const Matrix3f& intrinsics,
                                        size_t maxIters ) const
            {
                return ( ( Derived* )this )->computeRelativePose( predicted, gray, intrinsics, maxIters );
            }
    };

}

#endif // KEYFRAMEBASE_H
