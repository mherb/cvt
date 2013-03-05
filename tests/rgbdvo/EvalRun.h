#ifndef CONVERGENCETESTS_H
#define CONVERGENCETESTS_H

#include <cvt/util/ConfigFile.h>
#include <cvt/util/Time.h>
#include <cvt/vision/rgbdvo/RGBDVisualOdometry.h>
#include <cvt/io/RGBDParser.h>
#include <BatchEvaluation.h>

namespace cvt {

    template <class KFType, class LossFunc>
    class EvalRun
    {
        public:
            EvalRun( RGBDVisualOdometry<KFType, LossFunc>&vo, const String& folder, ConfigFile& cfg ) :
                _vo( vo ),
                _parser( folder, 0.03f )
            {
                _parser.setIdx( cfg.valueForName( "dataStartIdx", 0 ) );
                _parser.loadNext();

                const RGBDParser::RGBDSample& sample = _parser.data();
                _gray.reallocate( sample.rgb.width(), sample.rgb.height(), IFormat::GRAY_FLOAT );
                _depth.reallocate( sample.depth.width(), sample.depth.height(), IFormat::GRAY_FLOAT );
                convertCurrent();

                // add initial
                _vo.addNewKeyframe( _gray, _depth, sample.pose<float>() );
            }

            void evaluateDataSetPerformance( ConfigFile& /*cfg*/ )
            {
                BatchEvaluation evaluator;

                Matrix4f pose;
                pose.setIdentity();

                Time time;
                size_t iters = 0;
                float timeSum = 0;

                //float translationError = 0.0f;

                pose = _vo.pose();
                while( _parser.hasNext() ){
                    _parser.loadNext();
                    const RGBDParser::RGBDSample& d = _parser.data();

                    time.reset();
                    convertCurrent();
                    pose = _vo.pose();
                    _vo.updatePose( pose, _gray, _depth );

                    timeSum += time.elapsedMilliSeconds();
                    iters++;

                    if( d.poseValid ){
                        Matrix4f poseGT = d.pose<float>();
                        //float currError = ( poseGT - pose ).col( 3 ).length();

                        evaluator.add( d.stamp, pose, poseGT );

                        //float errorChange = currError - translationError;
                        //std::cout << "ErrorChange: " << errorChange << std::endl;
                        //translationError = currError;
                    }

                    std::cout << "\r" << _parser.iter() << " / " << _parser.size();
                    std::flush( std::cout );
                }

                evaluator.writeComputedFile( "trajectory.txt" );
                //evaluator.evalRPEPerSecond();
                std::cout << "Average Proc. Time per frame:\t " << timeSum / iters << "ms" << std::endl;
                std::cout << "Number of created Keyframes:\t "	<< _vo.numOverallKeyframes() << std::endl;
            }

        private:
            RGBDVisualOdometry<KFType, LossFunc>&    _vo;
            RGBDParser                               _parser;

            Image   _gray;
            Image   _depth;

            void convertCurrent()
            {
                const RGBDParser::RGBDSample& sample = _parser.data();
                sample.rgb.convert( _gray );
                sample.depth.convert( _depth );
            }
    };

}

#endif // CONVERGENCETESTS_H
