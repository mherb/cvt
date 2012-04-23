#ifndef CVT_MULTISCALEKEYFRAME_H
#define CVT_MULTISCALEKEYFRAME_H

#include <KeyframeBase.h>
#include <cvt/vision/ImagePyramid.h>

namespace cvt {

    template <class KFType>
    class MultiscaleKeyframe : public KeyframeBase<MultiscaleKeyframe<KFType> >
    {
        public:
            typedef typename KFType::Result Result;

            MultiscaleKeyframe( const Matrix4f& pose, const Matrix3f& K,
                                const Image& gray, const Image& depth, float depthScale ) :
                _grayPyramid( 3, 0.5f )
            {
                size_t  octaves  = 3;
                float   scalefac = 0.5f;

                // create one VOKeyframe per octave
                ImagePyramid pyrGray( octaves, scalefac );
                pyrGray.update( gray );

                ImagePyramid pyrDepth( octaves, scalefac );
                pyrDepth.update( depth );

                Matrix3f scaledK = K;
                for( size_t i = 0; i < octaves; i++ ){
                    _octaveKeyframes.push_back( new KFType( pyrGray[ i ], pyrDepth[ i ], pose, scaledK, depthScale ) );
                    scaledK *= scalefac;
                    scaledK[ 2 ][ 2 ] = 1.0f;
                }
            }

            ~MultiscaleKeyframe()
            {
                for( size_t i = 0; i < _octaveKeyframes.size(); i++ ){
                    if( _octaveKeyframes[ i ] ){
                        delete _octaveKeyframes[ i ];
                    }
                }
                _octaveKeyframes.clear();
            }

            const Matrix4f&     pose()                          const { return _octaveKeyframes[ 0 ]->pose(); }

            Result computeRelativePose( SE3<float>& predicted,
                                        const Image& gray,
                                        const Matrix3f& intrinsics,
                                        size_t maxIters ) const
            {
                _grayPyramid.update( gray );

                Result r;
                // for each scale, call the respective aligner
                Matrix3f scaledK;
                for( int i = _grayPyramid.octaves() - 1; i >= 0; i-- ){
                    scaledK = intrinsics * Math::pow( _grayPyramid.scaleFactor(), (float)i );
                    scaledK[ 2 ][ 2 ] = 1.0f;
                    r = _octaveKeyframes[ i ].computeRelativePose( predicted, _grayPyramid[ i ], scaledK, maxIters );
                }
                return r;
            }

        private:
            std::vector<KFType*>    _octaveKeyframes;
            ImagePyramid            _grayPyramid;
    };

}

#endif
