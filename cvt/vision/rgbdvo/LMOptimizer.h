/*
            CVT - Computer Vision Tools Library

     Copyright (c) 2012, Philipp Heise, Sebastian Klose

    THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
    KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
    IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
    PARTICULAR PURPOSE.
*/
#ifndef CVT_LMOPTIMIZER_H
#define CVT_LMOPTIMIZER_H

#include <cvt/vision/rgbdvo/Optimizer.h>

namespace cvt {

    template <class WarpFunc, class LossFunc>
    class LMOptimizer : public Optimizer<WarpFunc, LossFunc>
    {
        public:
            LMOptimizer();
            ~LMOptimizer(){}


        private:
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
    };

    template <class WarpFunc, class LossFunc>
    inline LMOptimizer<WarpFunc, LossFunc>::LMOptimizer() :
        Base()
    {
    }

    template <class WarpFunc, class LossFunc>
    inline void LMOptimizer<WarpFunc, LossFunc>::optimizeSingleScale( ResultType& result,
                                                                      ReferenceType& reference,
                                                                      const Image& gray,
                                                                      const Image& /*depthImage*/,
                                                                      size_t octave )
    {
        JacobianType deltaSum;
        HessianType  hessian;
        float lambda = 0.01f;

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
        result.numPixels = residuals.size();
        WarpFunc savedWarp( result.warp );

        if( this->_logError ){
            this->_logger.log( octave, result.iterations, result.warp.pose() );
        }

        HessianType hTmp;
        while( result.iterations < this->_maxIter ){
            // compute the step:
            hTmp = hessian;

            // multiplicative damping
            hTmp.diagonal() *= ( 1.0f + lambda );
            DeltaType deltaP = -hTmp.inverse() * deltaSum.transpose();

            if( deltaP.norm() < this->_minUpdate )
                break;
            result.warp.updateParameters( deltaP );

            // re-evaluate the cost function
            reference.recompute( residuals, jacobians, result.warp, grayMap, octave );
            float currentCosts = simd->sumSqr( &residuals[ 0 ], residuals.size() );

            if( !residuals.size() ){
                result.warp = savedWarp;
                break;
            }

            if( currentCosts < result.costs ){
                // update the system:
                this->_overallDelta.noalias() += deltaP;
                result.costs = Base::evaluateSystem( hessian, deltaSum, &jacobians[ 0 ], &residuals[ 0 ], residuals.size() );
                savedWarp = result.warp;
                lambda *= 0.5f;
                result.iterations++;
                result.numPixels = residuals.size();

                if( this->_logError ){
                    this->_logger.log( octave, result.iterations, result.warp.pose() );
                }

                if( result.costs / result.numPixels < Base::_costStopThreshold ){
                    // stop optimization, costs have reached sufficient minimum
                    break;
                }
            } else {
                // undo the step
                result.warp = savedWarp;

                // update the damping
                lambda *= 10.0f;
            }
        }

        result.pixelPercentage = ( float )result.numPixels / ( float )reference.dataSize( octave );
    }

}


#endif // LMOPTIMIZER_H
