#ifndef CONVERGENCETESTS_H
#define CONVERGENCETESTS_H

#include <cvt/util/ConfigFile.h>
#include <cvt/vision/rgbdvo/Optimizer.h>
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
                parser.setIdx( cfg.valueForName( "dataStartIdx", 0 ) );
                parser.loadNext();

                const RGBDParser::RGBDSample& sample = parser.data();
                _gray.reallocate( sample.rgb.width(), sample.rgb.height(), IFormat::GRAY_FLOAT );
                _depth.reallocate( sample.depth.width(), sample.depth.height(), IFormat::GRAY_FLOAT );
                convertCurrent();

                // add initial
                _vo.addNewKeyframe( _gray, _depth, sample.pose<float>() );
            }

            void evaluateConvergenceArea( ConfigFile& cfg )
            {
            }

            void evaluateDataSetPerformance( ConfigFile& cfg )
            {
                BatchEvaluation evaluator;

                Matrix4f pose;
                pose.setIdentity();

                Time time;
                size_t iters = 0;
                float timeSum = 0;

                //float translationError = 0.0f;

                pose = vo.pose();
                while( parser.hasNext() ){
                    parser.loadNext();
                    const RGBDParser::RGBDSample& d = parser.data();

                    time.reset();
                    d.rgb.convert( gray );
                    d.depth.convert( depth );
                    pose = vo.pose();
                    vo.updatePose( pose, gray, depth );

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

                    std::cout << "\r" << parser.iter() << " / " << parser.size();
                    std::flush( std::cout );
                }

                evaluator.writeComputedFile( "trajectory.txt" );
                evaluator.evalRPEPerSecond();

                std::cout << "Average Proc. Time per frame:\t " << timeSum / iters << "ms" << std::endl;
                std::cout << "Number of created Keyframes:\t "	<< vo.numOverallKeyframes() << std::endl;
            }

        private:
            RGBDVisualOdometry<KFType, LossFunc>&    _vo;
            RGBDParser                               _parser;

            Image   _gray;
            Image   _depth;

            void convertCurrent()
            {
                const RGBDParser::RGBDSample& sample = parser.data();
                sample.rgb.convert( _gray );
                sample.depth.convert( _depth );
            }
    };

}

#endif // CONVERGENCETESTS_H
