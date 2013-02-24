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


    template <class WarpFunc, class LossFunc>
    class LMOptimizer : public Optimizer<WarpFunc, LossFunc>
    {
        public:
            LMOptimizer();
            ~LMOptimizer(){}


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
            float evaluateSystem( HessianType& hessian, JacobianType& deltaSum,
                                  const JacobianType* jacobians, const float* residuals, size_t n );

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
        JacobianType dampingFactor( JacobianType::Ones() );
        dampingFactor *= 0.01f;

        IMapScoped<const float> grayMap( gray );

        result.iterations = 0;
        result.numPixels = 0;
        result.pixelPercentage = 0.0f;

        SIMD* simd = SIMD::instance();

        std::vector<float> residuals;
        JacobianVec jacobians;

        // initial costs
        reference.recompute( residuals, jacobians, result.warp, grayMap, octave );
        result.costs = evaluateSystem( hessian, deltaSum, &jacobians[ 0 ], &residuals[ 0 ], residuals.size() );
        result.numPixels = residuals.size();
        WarpFunc savedWarp( result.warp );

        HessianType hTmp;
        while( result.iterations < this->_maxIter ){
            // compute the step:
            hTmp = hessian;
            hTmp.diagonal() += dampingFactor;
            DeltaType deltaP = -hTmp.inverse() * deltaSum.transpose();

            if( deltaP.norm() < this->_minUpdate )
                break;
            result.warp.updateParameters( deltaP );

            // re-evaluate the cost function
            reference.recompute( residuals, jacobians, result.warp, grayMap, octave );
            float currentCosts = simd->sumSqr( &residuals[ 0 ], residuals.size() );

            if( !residuals.size() ){
                break;
            }

            if( currentCosts < result.costs ){
                // update the system:
                result.costs = evaluateSystem( hessian, deltaSum, &jacobians[ 0 ], &residuals[ 0 ], residuals.size() );
                savedWarp = result.warp;
                dampingFactor *= 0.5f;
                result.iterations++;
                result.numPixels = residuals.size();

                if( result.costs / result.numPixels < Base::_costStopThreshold ){
                    // stop optimization, costs have reached sufficient minimum
                    break;
                }
            } else {
                // undo the step
                result.warp = savedWarp;

                // update the damping
                dampingFactor *= 10.0f;
            }
        }

        result.pixelPercentage = ( float )result.numPixels / ( float )reference.dataSize( octave );
    }

    template <class WarpFunc, class LossFunc>
    inline float LMOptimizer<WarpFunc, LossFunc>::evaluateSystem( HessianType& hessian, JacobianType& deltaSum,
                                                                  const JacobianType* jacobians, const float* residuals, size_t n  )
    {
        float median = this->computeMedian( residuals, n );
        float mad = this->computeMAD( residuals, n, median );

        // this is an estimate for the standard deviation:
        this->_weighter.setScale( 1.4826f * mad );
        return Base::_builder.build( hessian,
                                     deltaSum,
                                     jacobians,
                                     residuals,
                                     n );
    }

}


#endif // LMOPTIMIZER_H
