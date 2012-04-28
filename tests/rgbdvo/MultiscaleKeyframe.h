#ifndef CVT_MULTISCALEKEYFRAME_H
#define CVT_MULTISCALEKEYFRAME_H

#include <KeyframeBase.h>
#include <cvt/vision/ImagePyramid.h>

namespace cvt {

    template <class KFType>
    class MultiscaleKeyframe : public KeyframeBase<MultiscaleKeyframe<KFType> >
    {
        public:
            MultiscaleKeyframe( const Image& gray, const Image& depth,
                                const Matrix4f& pose, const Matrix3f& K,
                                const VOParams& params ) :
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
                    _octaveKeyframes.push_back( new KFType( pyrGray[ i ], pyrDepth[ i ], pose, scaledK, params ) );
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

            const Matrix4f&     pose()  const { return _octaveKeyframes[ 0 ]->pose(); }

            VOResult computeRelativePose( PoseRepresentation& predicted,
                                          const Image& gray,
                                          const Matrix3f& intrinsics,
                                          const VOParams& params ) const
            {
                _grayPyramid.update( gray );

                VOResult r;
                // for each scale, call the respective aligner
                Matrix3f scaledK;
                for( int i = _grayPyramid.octaves() - 1; i >= 0; i-- ){
                    scaledK = intrinsics * Math::pow( _grayPyramid.scaleFactor(), (float)i );
                    scaledK[ 2 ][ 2 ] = 1.0f;
                    r = _octaveKeyframes[ i ]->computeRelativePose( predicted, _grayPyramid[ i ], scaledK, params );
                }
                return r;
            }

        private:
            std::vector<KFType*>    _octaveKeyframes;
            mutable ImagePyramid    _grayPyramid;
    };

}

#endif
