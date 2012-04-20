#ifndef CVT_RGBDSCALESPACEALIGNMENT_H
#define CVT_RGBDSCALESPACEALIGNMENT_H

#include <cvt/gfx/Image.h>
#include <cvt/vision/ImagePyramid.h>
#include <cvt/math/SE3.h>
#include <RGBDAlignment.h>
#include <MultiscaleKeyframe.h>
#include <vector>

namespace cvt {

    class RGBDScaleSpaceAlignment
    {
        public:
            RGBDScaleSpaceAlignment( const Matrix3f& K, size_t iterationPerScale, float depthScale, size_t nOctaves, float scaleFactor );
            ~RGBDScaleSpaceAlignment();

            void            alignWithKeyframe( SE3<float>& pose, const MultiscaleKeyframe& kf, const Image& gray );

            const Matrix3f& intrinsics( size_t octave = 0 )  const { return _alignerForOctave[ octave ].intrinsics(); }

        private:
            size_t                      _octaves;
            float                       _scaleFactor;
            std::vector<RGBDAlignment>  _alignerForOctave;
            ImagePyramid                _grayPyramid;
    };

}

#endif // RGBDSCALESPACEALIGNMENT_H
