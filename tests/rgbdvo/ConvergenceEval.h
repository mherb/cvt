#ifndef CVT_CONVERGENCEEVAL_H
#define CVT_CONVERGENCEEVAL_H

#include <cvt/io/RGBDParser.h>
#include <cvt/vision/rgbdvo/Optimizer.h>
#include <cvt/vision/rgbdvo/RGBDKeyframe.h>
#include <cvt/vision/ImagePyramid.h>
#include <vector>
#include <fstream>

namespace cvt {

    struct ConvergenceResult
    {
        float rotationDist;
        float translationDist;
        float finalRotationError;
        float finalTranslationError;
        float errorMix;
        bool success;
    };

    template <class KFType, class LossFunc>
    class ConvergenceEval
    {
        public:
            typedef typename KFType::WarpType           Warp;
            typedef Optimizer<Warp, LossFunc>  OptimizerType;

            ConvergenceEval( OptimizerType* optimizer,
                             const Matrix3f& K,
                             const String& folder,
                             ConfigFile& cfg ) :
                _parser( folder, 0.03f ),
                _optimizer( optimizer ),
                _reference( K, cfg.valueForName<int>( "pyrOctaves", 3 ), cfg.valueForName<float>( "pyrScale", 0.5f ) ),
                _pyramid( cfg.valueForName<int>( "pyrOctaves", 3 ), cfg.valueForName<float>( "pyrScale", 0.5f ) ),
                _angleWeight( cfg.valueForName<float>( "conv_area_success_thresh", 0.02f ) ),
                _successThresh( cfg.valueForName<float>( "conv_area_angle_weight", 0.1f ) )
            {
                _reference.setDepthMapScaleFactor( cfg.valueForName<float>( "depthFactor", 1000.0f ) *
                                                   cfg.valueForName<float>( "depthScale", 1.0f ) );
                _reference.setMinimumDepth( cfg.valueForName<float>( "minDepth", 0.5f ) );
                _reference.setGradientThreshold( cfg.valueForName<float>( "gradientThreshold", 0.01f ) );
                _reference.setSelectionPixelPercentage( cfg.valueForName<float>( "selectionPixelPercentage", 0.3f ) );
                _reference.setUseInformationSelection( cfg.valueForName<bool>( "useInformationSelection", false ) );

                _optimizer->setMaxIterations( cfg.valueForName<int>( "maxIters", 10 ) );
                _optimizer->setMinPixelPercentage( cfg.valueForName<float>( "minPixelPercentage", 0.3f ) );
                _optimizer->setMinUpdate( cfg.valueForName<float>( "minParameterUpdate", 1e-6f ) );

                _angleExtent[ 0 ] = Math::PI;
                _angleExtent[ 1 ] = 1.0f;
                _tExtent[ 0 ] = 1000.0f;
                _tExtent[ 1 ] = 0.0f;
            }

            void evaluate()
            {
                size_t refIdx = 0;

                while( refIdx < _parser.size() ){
                    _parser.setIdx( refIdx );
                    _parser.loadNext();
                    const RGBDParser::RGBDSample& sample = _parser.data();

                    sample.rgb.convert( _grayf, IFormat::GRAY_FLOAT );
                    sample.depth.convert( _depthf, IFormat::GRAY_FLOAT );
                    if( sample.poseValid ){
                        Matrix4f pose = sample.pose<float>();
                        _pyramid.update( _grayf );

                        // update the current reference frame
                        _reference.updateOfflineData( pose, _pyramid, _depthf );

                        evalConvergenceForCurrentRef( refIdx );
                    }

                    refIdx += 50;

                    std::cout.precision( 2 );
                    std::cout << "\r" << std::fixed << 100.0f * (float)refIdx / (float)_parser.size() << "%";
                    std::flush( std::cout );
                }

                saveData();
            }

        private:
            RGBDParser      _parser;
            OptimizerType*  _optimizer;
            KFType          _reference;


            ImagePyramid    _pyramid;
            Image           _grayf;
            Image           _depthf;

            std::vector<ConvergenceResult>  _results;
            float                           _angleWeight;
            float                           _successThresh;
            Vector2f                        _angleExtent;
            Vector2f                        _tExtent;

            void evalConvergenceForCurrentRef( size_t startIdx )
            {
                // move from startIdx backward
                int idx = startIdx;
                size_t seqFails = 0;
                while( idx >= 0 && seqFails < 5 ){
                    if( !computeAlignment( idx ) ){
                        seqFails++;
                    }
                    idx--;
                }

                // move from startIdx forward
                idx = startIdx;
                seqFails = 0;
                while( idx < _parser.size() && seqFails < 5 ){
                    if( !computeAlignment( idx ) ){
                        seqFails++;
                    }
                    idx++;
                }
            }

            bool computeAlignment( size_t idx )
            {
                _parser.setIdx( idx );
                _parser.loadNext();
                const RGBDParser::RGBDSample& sample = _parser.data();

                Matrix4f posePrediction = _reference.pose();

                typename OptimizerType::Result result;

                sample.rgb.convert( _grayf, IFormat::GRAY_FLOAT );
                sample.depth.convert( _depthf, IFormat::GRAY_FLOAT );
                ConvergenceResult convres;
                if( sample.poseValid ){
                    _pyramid.update( _grayf );

                    _optimizer->optimize( result, posePrediction, _reference, _pyramid, _depthf );

                    Matrix4f estimated = result.warp.pose();
                    Matrix4f gtPose = sample.pose<float>();
                    computeErrors( convres, gtPose, estimated, posePrediction );
                    _results.push_back( convres );

                    _angleExtent[ 0 ] = Math::min( _angleExtent[ 0 ], convres.rotationDist );
                    _angleExtent[ 1 ] = Math::max( _angleExtent[ 1 ], convres.rotationDist );
                    _tExtent[ 0 ] = Math::min( _tExtent[ 0 ], convres.translationDist );
                    _tExtent[ 1 ] = Math::max( _tExtent[ 1 ], convres.translationDist );

                    return convres.success;
                }
                return false;
            }

            void computeErrors( ConvergenceResult& r,
                                const Matrix4f& gt,
                                const Matrix4f& estimated,
                                const Matrix4f& reference )
            {
                // relative distance of start to ref
                Matrix4f relStart = reference.inverse() * gt;
                // error
                Matrix4f error = estimated.inverse() * gt;

                r.rotationDist = angleDistance( relStart );
                r.translationDist = transDistance( relStart );
                r.finalRotationError = angleDistance( error );
                r.finalTranslationError = transDistance( error );

                r.errorMix = Math::mix( r.rotationDist, r.translationDist, _angleWeight );
                r.success = ( r.errorMix ) < _successThresh;
            }

            float angleDistance( const Matrix4f& e )
            {
                Quaternionf q( e.toMatrix3() );
                return q.toEuler().length();
            }

            float transDistance( const Matrix4f& e )
            {
                return Math::sqrt( e.col( 3 ).lengthSqr() - 1.0f );
            }

            void saveData() const
            {
                std::ofstream file;
                file.open( "conv_area.txt" );
                file.precision( 15 );
                file << "# <angleDist> <tDist> <angleError> <tError> <success>" << std::endl;

                for( size_t k = 0; k < _results.size(); k++ ){
                    const ConvergenceResult& cr = _results[ k ];
                    file << std::fixed
                         << cr.rotationDist << " "
                         << cr.translationDist << " "
                         << cr.finalRotationError << " "
                         << cr.finalTranslationError << " "
                         << std::noboolalpha << cr.success << " "
                         << std::endl;
                }
                file.close();
            }
    };

}

#endif
