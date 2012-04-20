#include <MultiscaleKeyframe.h>

#include <cvt/vision/ImagePyramid.h>

namespace cvt {

    MultiscaleKeyframe::MultiscaleKeyframe( const Matrix4f& pose, const Matrix3f& K, const Image& gray, const Image& depth, float depthScale, size_t octaves, float scalefac )
    {
        // create one VOKeyframe per octave
        ImagePyramid pyrGray( octaves, scalefac );
        pyrGray.update( gray );

        ImagePyramid pyrDepth( octaves, scalefac );
        pyrGray.update( depth );

        Matrix3f scaledK = K;

        for( size_t i = 0; i < octaves; i++ ){
            _octaveKeyframes.push_back( new VOKeyframe( pyrGray[ i ], pyrDepth[ i ], pose, scaledK, depthScale ) );
            scaledK *= scalefac;
            scaledK[ 2 ][ 2 ] = 1.0f;
        }
    }

    MultiscaleKeyframe::~MultiscaleKeyframe()
    {
        for( size_t i = 0; i < _octaveKeyframes.size(); i++ ){
            if( _octaveKeyframes[ i ] ){
                delete _octaveKeyframes[ i ];
            }
        }
        _octaveKeyframes.clear();
    }



}
