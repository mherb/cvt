#ifndef CVT_MULTISCALEKEYFRAME_H
#define CVT_MULTISCALEKEYFRAME_H

#include <VOKeyframe.h>

namespace cvt {

    class MultiscaleKeyframe
    {
        public:
            MultiscaleKeyframe( const Matrix4f& pose, const Matrix3f& K,
                                const Image& gray, const Image& depth, float depthScale = 5000.0f,
                                size_t octaves = 3, float scalefac = 0.5f );

            ~MultiscaleKeyframe();

            const VOKeyframe&   keyframeForOctave( size_t idx ) const { return *_octaveKeyframes[ idx ]; }
            const Matrix4f&     pose()                          const { return _octaveKeyframes[ 0 ]->pose(); }

        private:
            std::vector<VOKeyframe*>    _octaveKeyframes;

    };

}

#endif
