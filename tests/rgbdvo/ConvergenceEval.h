#ifndef CVT_CONVERGENCEEVAL_H
#define CVT_CONVERGENCEEVAL_H

#include <cvt/io/RGBDParser.h>
#include <cvt/vision/rgbdvo/Optimizer.h>
#include <cvt/vision/rgbdvo/RGBDKeyframe.h>
#include <cvt/vision/ImagePyramid.h>
#include <cvt/math/Quaternion.h>
#include <vector>
#include <fstream>

namespace cvt {

    struct ConvergenceResult
    {
        Vector3f deltaRotFinal;
        Vector3f deltaTFinal;
        Vector3f deltaRotInitial;
        Vector3f deltaTInitial;

        bool success;
    };

    template <class KFType, class LossFunc>
    class ConvergenceEval
    {
        public:
            typedef typename KFType::WarpType           Warp;
            typedef Optimizer<Warp, LossFunc>  OptimizerType;
            typedef typename OptimizerType::Result ResultType;

            ConvergenceEval( OptimizerType* optimizer,
                             const Matrix3f& K,
                             const String& folder,
                             ConfigFile& cfg ) :
                _parser( folder, 0.02f ),
                _optimizer( optimizer ),
                _reference( K, cfg.valueForName<int>( "pyrOctaves", 3 ), cfg.valueForName<float>( "pyrScale", 0.5f ) ),
                _pyramid( cfg.valueForName<int>( "pyrOctaves", 3 ), cfg.valueForName<float>( "pyrScale", 0.5f ) ),
                _numSamples( cfg.valueForName<int>( "eval_num_samples", 5000 ) ),
                _angleRange( cfg.valueForName<float>( "eval_angle_range", 10.0f ) ),
                _tRange( cfg.valueForName<float>( "eval_trans_range", 1.0 ) )
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

                size_t idx = cfg.valueForName<int>( "data_idx", 200 );
                if( idx < _parser.size() ){
                    _parser.setIdx( idx );
                } else {
                    throw CVTException( "Index out of bounds" );
                }

                _parser.loadNext();
                Matrix4f pose;
                pose.setIdentity();
                const RGBDParser::RGBDSample& sample = _parser.data();
                sample.rgb.convert( _grayf, IFormat::GRAY_FLOAT );
                sample.depth.convert( _depthf, IFormat::GRAY_FLOAT );
                _pyramid.update( _grayf );
                _reference.updateOfflineData( pose, _pyramid, _depthf );
            }

            void evaluate()
            {
                Matrix4f deltaPose;

                ResultType result;

                size_t n = 0;
                while( n < _numSamples ){
                    // generate an offset pose
                    deltaPose.setRotationXYZ( Math::deg2Rad( Math::rand( -_angleRange, _angleRange ) ),
                                              Math::deg2Rad( Math::rand( -_angleRange, _angleRange ) ),
                                              Math::deg2Rad( Math::rand( -_angleRange, _angleRange ) ) );
                    deltaPose[ 0 ][ 3 ] = Math::rand( -_tRange, _tRange );
                    deltaPose[ 1 ][ 3 ] = Math::rand( -_tRange, _tRange );
                    deltaPose[ 2 ][ 3 ] = Math::rand( -_tRange, _tRange );

                    _optimizer->optimize( result, deltaPose, _reference, _pyramid, _depthf );

                    // log the result
                    addResult( deltaPose, result );

                    std::cout.precision( 2 );
                    std::cout << "\r" << std::fixed << 100.0f * (float)n / (float)_numSamples << "%";
                    std::flush( std::cout );
                    n++;
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
            size_t          _numSamples;
            float           _angleRange;
            float           _tRange;

            std::vector<ConvergenceResult>  _results;

            void addResult( const Matrix4f& initial, const ResultType& result )
            {
                _results.push_back( ConvergenceResult() );

                Matrix4f p = result.warp.pose();

                Quaternionf q( initial.toMatrix3() );

                ConvergenceResult& r = _results.back();
                r.deltaRotInitial = q.toEuler();
                r.deltaTInitial = Vector3f( initial.col( 3 ) );

                q = Quaternionf( p.toMatrix3() );
                r.deltaRotFinal = q.toEuler();
                r.deltaTFinal = Vector3f( p.col( 3 ) );
                r.success = result.success;
            }

            void saveData() const
            {
                std::ofstream file;
                file.open( "conv_area.txt" );
                file.precision( 15 );
                file << "# <3x d angle init> <3x d t initial> <3xangle final> <3x t final> <success>" << std::endl;

                for( size_t k = 0; k < _results.size(); k++ ){
                    const ConvergenceResult& cr = _results[ k ];
                    file << std::fixed
                         << cr.deltaRotInitial[ 0 ] << " "
                         << cr.deltaRotInitial[ 1 ] << " "
                         << cr.deltaRotInitial[ 2 ] << " "
                         << cr.deltaTInitial[ 0 ] << " "
                         << cr.deltaTInitial[ 1 ] << " "
                         << cr.deltaTInitial[ 2 ] << " "
                         << cr.deltaRotFinal[ 0 ] << " "
                         << cr.deltaRotFinal[ 1 ] << " "
                         << cr.deltaRotFinal[ 2 ] << " "
                         << cr.deltaTFinal[ 0 ] << " "
                         << cr.deltaTFinal[ 1 ] << " "
                         << cr.deltaTFinal[ 2 ] << " "
                         << std::noboolalpha << cr.success << " "
                         << std::endl;
                }
                file.close();
            }
    };

}

#endif
