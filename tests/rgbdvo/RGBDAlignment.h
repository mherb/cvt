#ifndef CVT_RGBD_ALIGNMENT_H
#define CVT_RGBD_ALIGNMENT_H

#include <cvt/math/SE3.h>
#include <VOKeyframe.h>

namespace cvt
{
    class RGBDAlignment
    {
        public:
            RGBDAlignment( const Matrix3f& K, size_t maxIterations, float depthScale = 5000.0f );
            ~RGBDAlignment();

            /**
             *  \brief align a given image pair (rgb + depth)
             */
            void alignWithKeyFrame( SE3<float>& predicted,
                                    const VOKeyframe& keyframe,
                                    const Image& rgb,
                                    const Image& depth );

            const Matrix3f& intrinsics() const { return _intrinsics; }

        private:
            Matrix3f    _intrinsics;
            size_t      _maxIterations;
            float       _depthScale;

    };
}

#endif
