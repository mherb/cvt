#ifndef LMOPTIMIZER_H
#define LMOPTIMIZER_H

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
    class LMOptimizer : public Optimizer<WarpFunc, Weighter>
    {
        public:
            typedef Optimizer<WarpFunc, Weighter>  Base;
            typedef typename Base::ResultType      ResultType;
            LMOptimizer();
            ~LMOptimizer(){}


            void optimize( ResultType& result,
                           const Matrix4f& posePrediction,
                           RGBDKeyframe<WarpFunc>& reference,
                           const ImagePyramid& grayPyramid,
                           const Image& depthImage ) const;

    };

    template <class WarpFunc, class LossFunc>
    inline LMOptimizer<WarpFunc, LossFunc>::LMOptimizer()
    {
    }

    template <class WarpFunc, class LossFunc>
    inline void LMOptimizer<WarpFunc, LossFunc>::optimize( ResultType& result,
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
        result.iterationsOnOctave.resize( grayPyramid.octaves(), 0 );
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
        typename Base::HessianType  damping( Base::HessianType::Identity() );

        const float costThreshold = 0.0002;

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

            scaleResult.iterationsOnOctave[ o ] = 0;
            scaleResult.numPixels = 0;
            scaleResult.pixelPercentage = 0.0f;

            Matrix4f tmpPose = scaleResult.warp.poseMatrix();
            float lambda = 0.001f;
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

            while( scaleResult.iterationsOnOctave[ o ] < Base::_maxIter ){
                // compute the update:
                typename Base::DeltaType deltaP = -( hessian + lambda * damping ).inverse() * deltaSum.transpose();
                scaleResult.warp.updateParameters( deltaP );

                // build the updated projection Matrix
                projMat = K4 * scaleResult.warp.poseMatrix();
                // project the points:
                simd->projectPoints( &warpedPts[ 0 ], projMat, p3dPtr, num );

                // interpolate the pixel values
                simd->warpBilinear1f( &interpolatedPixels[ 0 ], &warpedPts[ 0 ].x, grayMap.ptr(), grayMap.stride(), width, height, 0.5f, num );
                scaleResult.warp.computeResiduals( &residuals[ 0 ], referencePixVals, &interpolatedPixels[ 0 ], num );
                float currentCosts = scaleResult.warp.costs( &residuals[ 0 ], num );

                //std::cout << "cc: " << currentCosts << " lc: " << lastCosts << " lambda: " << lambda << std::endl;

                if( currentCosts < lastCosts ){
                    // keep the step, update model and lambda
                    median = this->computeMedian( &residuals[ 0 ], num );
                    weighter.setSigma( 1.4f * median ); /* this is an estimate for the standard deviation */

                    /* a hack: the builder does not touch the hessian if its a non robust lossfunc!*/
                    hessian = data.hessian;
                    scaleResult.numPixels = builder.build( hessian, deltaSum,
                                                           referenceJ,
                                                           &residuals[ 0 ],
                                                           scaleResult.costs,
                                                           num );
                    lastCosts = scaleResult.costs / scaleResult.numPixels;

                    /*std::cout << "Scale:\t" << o << "\tCosts:\t" << lastCosts << "\tDelta:" <<
                                 deltaP[ 0 ] << ", " <<
                                 deltaP[ 1 ] << ", " <<
                                 deltaP[ 2 ] << ", " <<
                                 deltaP[ 3 ] << ", " <<
                                 deltaP[ 4 ] << ", " <<
                                 deltaP[ 5 ] << "\tlambda:" << lambda <<  std::endl;*/


                    lambda /= 2.0f;
                    scaleResult.iterationsOnOctave[ o ]++;
                    tmpPose = scaleResult.warp.poseMatrix();

                    if( deltaP.norm() < Base::_minUpdate ){
                        break;
                    }
                } else {
                    lambda *= 10.0f;
                    scaleResult.warp.setPose( tmpPose );
                }

                if( lastCosts < costThreshold || lambda > 1e15f )
                    break;
            }

           // std::cout << std::endl;

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


#endif // LMOPTIMIZER_H
