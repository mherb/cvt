/*
            CVT - Computer Vision Tools Library

     Copyright (c) 2012, Philipp Heise, Sebastian Klose

    THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
    KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
    IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
    PARTICULAR PURPOSE.
*/

#ifndef CVT_TROPTIMIZER_H
#define CVT_TROPTIMIZER_H

#include <cvt/vision/rgbdvo/Optimizer.h>

namespace cvt {

    template <class WarpFunc, class LossFunc>
    class TROptimizer : public Optimizer<WarpFunc, LossFunc>
    {
        public:
            TROptimizer();
            ~TROptimizer(){}

        protected:
            typedef typename WarpFunc::JacobianType         JacobianType;
            typedef typename WarpFunc::HessianType          HessianType;
            typedef typename WarpFunc::DeltaVectorType      DeltaType;
            typedef RGBDKeyframe<WarpFunc>                  ReferenceType;
            typedef Optimizer<WarpFunc, LossFunc>           Base;
            typedef typename ReferenceType::JacobianVec     JacobianVec;
            typedef typename Base::Result                   ResultType;
            typedef typename Base::AlignDataType            AlignDataType;

            void optimizeSingleScale( ResultType& result,
                                      ReferenceType& reference,
                                      const Image& gray,
                                      const Image& /*depthImage*/,
                                      size_t octave );

        private:
            float   _acceptStepThresh;
            float   _increaseTRThresh;
            float   _trIncreaseFactor;
            float   _trDecreaseFactor;
            float   _initialLambda;

    };

    template <class WarpFunc, class LossFunc>
    inline TROptimizer<WarpFunc, LossFunc>::TROptimizer() :
        _acceptStepThresh( 0.02f ),
        _increaseTRThresh( 0.1f ),
        _trIncreaseFactor( 2.0f ),
        _trDecreaseFactor( 0.1f ),
        _initialLambda( 2.0f )
    {
    }

    template <class WarpFunc, class LossFunc>
    inline void TROptimizer<WarpFunc, LossFunc>::optimizeSingleScale( ResultType& result,
                                                                      ReferenceType& reference,
                                                                      const Image& gray,
                                                                      const Image& /*depthImage*/,
                                                                      size_t octave )
    {
        JacobianType deltaSum;
        HessianType  hessian;
        JacobianType step( JacobianType::Zero() );

        IMapScoped<const float> grayMap( gray );

        result.iterations = 0;
        result.numPixels = 0;
        result.pixelPercentage = 0.0f;

        SIMD* simd = SIMD::instance();

        std::vector<float> residuals;
        JacobianVec jacobians;

        // initial costs
        reference.recompute( residuals, jacobians, result.warp, grayMap, octave );
        result.costs = Base::evaluateSystem( hessian, deltaSum, &jacobians[ 0 ], &residuals[ 0 ], residuals.size() );
        // compute the step:
        DeltaType deltaP = -hessian.inverse() * deltaSum.transpose();
        // model for the expected decrease
        float expectedDecrease = -2.0f * deltaP.dot( deltaSum.transpose() );
        result.numPixels = residuals.size();

        WarpFunc savedWarp( result.warp );

        float lambda = _initialLambda;
        while( result.iterations < this->_maxIter ){

            if( deltaP.norm() < this->_minUpdate )
                break;

            // compute the step
            step = lambda * deltaP;
            result.warp.updateParameters( step );

            // re-evaluate the cost function
            reference.recompute( residuals, jacobians, result.warp, grayMap, octave );
            if( !residuals.size() ){
                result.warp = savedWarp;
                break;
            }

            float currentCosts = simd->sumSqr( &residuals[ 0 ], residuals.size() );
            float costFidelity = ( result.costs - currentCosts ) / ( expectedDecrease );

            if( costFidelity < _acceptStepThresh ){
                // step is worse than expected:
                // undo
                result.warp = savedWarp;
                lambda *= _trDecreaseFactor;
            } else {
                // accept step & update model
                this->_overallDelta.noalias() += deltaP;
                savedWarp = result.warp;
                result.costs = Base::evaluateSystem( hessian, deltaSum, &jacobians[ 0 ], &residuals[ 0 ], residuals.size() );
                result.numPixels = residuals.size();
                result.iterations++;

                // compute the step:
                deltaP = -hessian.inverse() * deltaSum.transpose();

                // model for the expected decrease
                expectedDecrease = -2.0f * deltaP.dot( deltaSum.transpose() );

                // very good step -> increase trust region
                if( costFidelity > _increaseTRThresh ){
                    lambda *= _trIncreaseFactor;
                }

                if( result.costs / result.numPixels < Base::_costStopThreshold ){
                    break;
                }
            }
            if ( lambda * deltaP.norm() < Base::_minUpdate ||
                 lambda > 1e15f ||
                 lambda < 1e-8f ){
                break;
            }
        }

        result.pixelPercentage = ( float )result.numPixels / ( float )reference.dataSize( octave );
    }


}

#endif // TROPTIMIZER_H
