#include <RGBDScaleSpaceAlignment.h>

namespace cvt {

    RGBDScaleSpaceAlignment::RGBDScaleSpaceAlignment( const Matrix3f& K, size_t iterationPerScale, float depthScale, size_t nOctaves, float scaleFactor ) :
        _scaleFactor( scaleFactor ),
        _grayPyramid( nOctaves, scaleFactor )
    {
        Matrix3f scaledK = K;
        for( size_t i = 0; i < nOctaves; i++ ){
            _alignerForOctave.push_back( RGBDAlignment( scaledK, iterationPerScale, depthScale ) );
            scaledK *= scaleFactor;
            scaledK[ 2 ][ 2 ] = 1.0f;
        }
    }

    RGBDScaleSpaceAlignment::~RGBDScaleSpaceAlignment()
    {
    }

    void RGBDScaleSpaceAlignment::alignWithKeyframe( SE3<float>& predictedPose, const MultiscaleKeyframe& kf, const Image& gray )
    {
        _grayPyramid.update( gray );

        // for each scale, call the respective aligner
        for( int i = _grayPyramid.octaves() - 1; i >= 0; i-- ){
            _alignerForOctave[ i ].alignWithKeyframe( predictedPose, kf.keyframeForOctave( i ), _grayPyramid[ i ] );
        }
    }

}
