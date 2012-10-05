#ifndef TROPTIMIZER_H
#define TROPTIMIZER_H

/*
            CVT - Computer Vision Tools Library

     Copyright (c) 2012, Philipp Heise, Sebastian Klose

    THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
    KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
    IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
    PARTICULAR PURPOSE.
*/

#include <cvt/vision/rgbdvo/Optimizer.h>


namespace cvt {

    template <class WarpFunc, class Weighter>
    class TROptimizer : public Optimizer<WarpFunc, Weighter>
    {
        public:
            typedef Optimizer<WarpFunc, Weighter>  Base;
            typedef typename Base::ResultType      ResultType;
            TROptimizer();
            ~TROptimizer(){}


            void optimize( ResultType& result,
                           const Matrix4f& posePrediction,
                           RGBDKeyframe<WarpFunc>& reference,
                           const ImagePyramid& grayPyramid,
                           const Image& depthImage ) const;

        private:
            float   _acceptStepThresh;
            float   _increaseTRThresh;
            float   _trIncreaseFactor;
            float   _trDecreaseFactor;
            float   _initialLambda;

    };

    template <class WarpFunc, class LossFunc>
    inline TROptimizer<WarpFunc, LossFunc>::TROptimizer() :
        _acceptStepThresh( 0.0f ),
        _increaseTRThresh( 0.1f ),
        _trIncreaseFactor( 2.0f ),
        _trDecreaseFactor( 0.1f ),
        _initialLambda( 2.0f )
    {
    }

    template <class WarpFunc, class LossFunc>
    inline void TROptimizer<WarpFunc, LossFunc>::optimize( ResultType& result,
                                                           const Matrix4f& posePrediction,
                                                           RGBDKeyframe<WarpFunc> &reference,
                                                           const ImagePyramid& grayPyramid,
                                                           const Image& /*depthImage*/ ) const
    {
        SIMD* simd = SIMD::instance();
        Matrix4f tmp4;
        tmp4 = posePrediction.inverse() * reference.pose();

        result.warp.setPose( tmp4 );
        result.costs = 0.0f;
        result.iterations = 0;
        result.numPixels = 0;
        result.pixelPercentage = 0.0f;

        /* TODO: robust statistics should use median of residuals for threshold */
        LossFunc weighter( Base::_robustThreshold );
        SystemBuilder<LossFunc> builder( weighter );

        Matrix4f projMat;
        std::vector<Vector2f> warpedPts;
        std::vector<float> interpolatedPixels;
        std::vector<float> residuals;
        // sum of jacobians * delta
        typename Base::JacobianType deltaSum;
        typename Base::HessianType  hessian;

        const float costThreshold = 0.001f;

        float expectedDecrease;

        for( int o = grayPyramid.octaves() - 1; o >= 0; o-- ){
            ResultType scaleResult;
            scaleResult = result;

            const size_t width = grayPyramid[ o ].width();
            const size_t height = grayPyramid[ o ].height();

            const typename Base::AlignDataType& data = reference.dataForScale( o );

            const size_t num = data.points3d.size();
            Matrix4f K4( data.intrinsics );
            const Vector3f* p3dPtr = &data.points3d[ 0 ];
            const float* referencePixVals = &data.pixelValues[ 0 ];
            const typename Base::JacobianType* referenceJ = &data.jacobians[ 0 ];

            warpedPts.resize( num );
            interpolatedPixels.resize( num );
            residuals.resize( num );

            IMapScoped<const float> grayMap( grayPyramid[ o ] );

            scaleResult.iterations = 0;
            scaleResult.numPixels = 0;
            scaleResult.pixelPercentage = 0.0f;

            float lastCosts = 1.0f;

            // eval the first time:
            // build the updated projection Matrix
            projMat = K4 * scaleResult.warp.poseMatrix();

            // project the points:
            simd->projectPoints( &warpedPts[ 0 ], projMat, p3dPtr, num );

            // interpolate the pixel values
            simd->warpBilinear1f( &interpolatedPixels[ 0 ], &warpedPts[ 0 ].x, grayMap.ptr(), grayMap.stride(), width, height, 0.5f, num );
            scaleResult.warp.computeResiduals( &residuals[ 0 ], referencePixVals, &interpolatedPixels[ 0 ], num );

            float median = this->computeMedian( &residuals[ 0 ], num );
            weighter.setSigma( 1.4f * median ); /* this is an estimate for the standard deviation */

            /* a hack: the builder does not touch the hessian if its a non robust lossfunc!*/
            hessian = data.hessian;
            scaleResult.numPixels = builder.build( hessian, deltaSum,
                                                   referenceJ,
                                                   &residuals[ 0 ],
                                                   scaleResult.costs,
                                                   num );
            // mean costs
            lastCosts = scaleResult.costs / scaleResult.numPixels;
            typename Base::DeltaType deltaP = -hessian.inverse() * deltaSum.transpose();
            typename Base::DeltaType step;

            // this is the model:
            expectedDecrease = -2.0f * deltaP.dot( deltaSum.transpose() );

            Matrix4f tmpPose = scaleResult.warp.poseMatrix();
            float lambda = _initialLambda;

            while( scaleResult.iterations < Base::_maxIter ){
                // compute the update:
                step = lambda * deltaP;
                scaleResult.warp.updateParameters( step );

                // build the updated projection Matrix
                projMat = K4 * scaleResult.warp.poseMatrix();
                // project the points:
                simd->projectPoints( &warpedPts[ 0 ], projMat, p3dPtr, num );

                // interpolate the pixel values
                simd->warpBilinear1f( &interpolatedPixels[ 0 ], &warpedPts[ 0 ].x, grayMap.ptr(), grayMap.stride(), width, height, 0.5f, num );
                scaleResult.warp.computeResiduals( &residuals[ 0 ], referencePixVals, &interpolatedPixels[ 0 ], num );
                float currentCosts = scaleResult.warp.costs( &residuals[ 0 ], num );

                float costFidelity = ( lastCosts - currentCosts ) / ( expectedDecrease );

                if( costFidelity < _acceptStepThresh ){
                    // step is worse than expected:
                    // undo
                    scaleResult.warp.setPose( tmpPose );
                    lambda *= _trDecreaseFactor;
                } else {
                    // keep step: update model
                    scaleResult.iterations++;
                    tmpPose = scaleResult.warp.poseMatrix();

                    median = this->computeMedian( &residuals[ 0 ], num );
                    weighter.setSigma( 1.4f * median ); /* this is an estimate for the standard deviation */

                    hessian = data.hessian;
                    scaleResult.numPixels = builder.build( hessian, deltaSum,
                                                           referenceJ,
                                                           &residuals[ 0 ],
                                                           scaleResult.costs,
                                                           num );
                    lastCosts = scaleResult.costs / scaleResult.numPixels;
                    deltaP = -hessian.inverse() * deltaSum.transpose();

                    expectedDecrease = -2.0f * deltaP.dot( deltaSum.transpose() );

                    // very good step? increase trust region
                    if( costFidelity > _increaseTRThresh ){
                        lambda *= _trIncreaseFactor;
                    }

                    if( lambda * deltaP.norm() < Base::_minUpdate ){
                        break;
                    }
                }

                if( lastCosts < costThreshold || lambda > 1e15f || lambda < 1e-8f )
                    break;
            }

            if( scaleResult.numPixels )
                scaleResult.pixelPercentage = ( float )scaleResult.numPixels / ( float )num;

            // TODO: ensure the result on this scale is good enough (pixel percentage & error )
            result = scaleResult;
        }

        tmp4 = result.warp.poseMatrix();
        tmp4 = reference.pose() * tmp4.inverse();
        result.warp.setPose( tmp4 );
    }

}

#endif // TROPTIMIZER_H
