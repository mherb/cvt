/*
			CVT - Computer Vision Tools Library

 	 Copyright (c) 2012, Philipp Heise, Sebastian Klose

 	THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 	KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 	IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 	PARTICULAR PURPOSE.
*/

#ifndef CVT_ESMALIGNMENT_H
#define CVT_ESMALIGNMENT_H

#include <ESMKeyframe.h>
#include <cvt/math/SE3.h>

namespace cvt {

    class ESMAlignment
    {
        public:
            struct Result
            {
                float  SSD;
                size_t numPixels;
                size_t iterations;
            };

            ESMAlignment( const Matrix3f& K, size_t maxIterations, float depthScale );
            ~ESMAlignment();

            /**
             *  \brief align a given image pair (rgb + depth)
             */
            Result alignWithKeyframe( SE3<float>& predicted,
                                      const ESMKeyframe& keyframe,
                                      const Image& gray );

            const Matrix3f& intrinsics() const { return _intrinsics; }

        private:
            Matrix3f    _intrinsics;
            size_t      _maxIterations;
            float       _depthScale;

            float   interpolatePixelValue( const Vector2f& pos, const float* ptr, size_t stride );
            bool    evalGradient( Vector2f& grad, const Vector2f* positions, const float* ptr, size_t stride, size_t w, size_t h );
    };
}
#endif
