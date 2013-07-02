/*
            CVT - Computer Vision Tools Library

     Copyright (c) 2012, Philipp Heise, Sebastian Klose

    THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
    KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
    IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
    PARTICULAR PURPOSE.
*/

#ifndef CVT_DLOPTIMIZER_H
#define CVT_DLOPTIMIZER_H

#include <cvt/vision/rgbdvo/Optimizer.h>
#include <Eigen/Dense>

namespace cvt {

    template <class WarpFunc, class LossFunc>
    class DLOptimizer : public Optimizer<WarpFunc, LossFunc>
    {
        public:
            DLOptimizer();
            ~DLOptimizer(){}

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

            using Base::optimizeSingleScale;

        private:
            float   _trIncreaseFactor;
            float   _trDecreaseFactor;

            /**
             * @brief computeSteepestDescentStep
             * @param sdStep result: the computed steepest descent step
             * @param j      vector containing the jacobians per point
             * @param g      the gradient: J^T r = jSum
             */
            void computeSteepestDescentStep( DeltaType& sdStep, const HessianType& h, const DeltaType& g ) const;
            void computeGaussNewtonStep( DeltaType& gnStep, const HessianType& h, const DeltaType& g ) const;
            void computeMixedStep(DeltaType& dlStep, const DeltaType& gnStep, const DeltaType& sdStep , float trustRadius) const;
            float updateTrustRadius( float radius, float fidelity, float tLow, float tHigh ) const;

    };

    template <class WarpFunc, class LossFunc>
    inline DLOptimizer<WarpFunc, LossFunc>::DLOptimizer() :
        _trIncreaseFactor( 2.0f ),
        _trDecreaseFactor( 0.1f )
    {
    }

    template <class WarpFunc, class LossFunc>
    inline void DLOptimizer<WarpFunc, LossFunc>::optimizeSingleScale( ResultType& result,
                                                                      ReferenceType& reference,
                                                                      const Image& gray,
                                                                      const Image& /*depthImage*/,
                                                                      size_t octave )
    {
        JacobianType deltaSum;
        HessianType  hessian;

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

        DeltaType sdStep( DeltaType::Zero() ), gnStep( DeltaType::Zero() ), dlStep( DeltaType::Zero() );

        // initial trust region radius
        float trustRegionRadius = 1.0f;

        if( this->_logError ){
            this->_logger.log( octave, result.iterations, result.warp.pose() );
        }

        // save the last result to reset pose
        WarpFunc savedWarp( result.warp );

        while( result.iterations < this->_maxIter &&
               result.costs > this->_costStopThreshold ){
            result.iterations++;

            float mc = deltaSum.norm();
            if( mc < this->_minUpdate ){
                break;
            }

            // compute the gradient descent step
            computeSteepestDescentStep( sdStep, hessian, deltaSum.transpose() );

            bool gnStepEvaluated = false;
            float fidelity = 0.0f;
            do {
                float nsd = sdStep.norm();
                if( nsd >= trustRegionRadius ){
                    // set the dog leg step to the scale trust region step
                    dlStep = trustRegionRadius / nsd * sdStep;
                } else {
                    if( !gnStepEvaluated ){
                        // compute GN step
                        computeGaussNewtonStep( gnStep, hessian, deltaSum.transpose() );
                        gnStepEvaluated = true;
                    }
                    float gnNorm = gnStep.norm();
                    if( gnNorm <= trustRegionRadius ){
                        // gn step lies inside the TR
                        dlStep = gnStep;
                    } else {
                        // mix the steps, solve for beta s.t. the step size is in the TR
                        computeMixedStep( dlStep, gnStep, sdStep, trustRegionRadius );
                    }
                }

                // apply the step
                result.warp.updateParameters( dlStep );

                // evaluate the new costs
                reference.recompute( residuals, jacobians, result.warp, grayMap, octave );
                if( !residuals.size() ){
                    result.warp = savedWarp;
                    fidelity = -1.0f;
                } else {
                    // evaluate the fidelity: actual cost change vs. expected model cost change
                    float expectedDecrease = -2.0f * dlStep.dot( deltaSum.transpose() );
                    float currentCosts = simd->sumSqr( &residuals[ 0 ], residuals.size() );
                    fidelity = ( result.costs - currentCosts ) / ( expectedDecrease );

                    if( fidelity > 0.0f ){
                        // re-evaluate system
                        this->_overallDelta.noalias() += dlStep;
                        savedWarp = result.warp;
                        result.costs = Base::evaluateSystem( hessian, deltaSum, &jacobians[ 0 ], &residuals[ 0 ], residuals.size() );
                        result.numPixels = residuals.size();
                    } else {
                        result.warp = savedWarp;
                    }
                }

                // update the radius
                trustRegionRadius = updateTrustRadius( trustRegionRadius, fidelity, 0.25f, 0.75f );

            } while( dlStep.norm() > this->_minUpdate &&
                     fidelity <= 0.0f &&
                     trustRegionRadius > this->_minUpdate );

            if( this->_logError ){
                this->_logger.log( octave, result.iterations, result.warp.pose() );
            }

            if( result.costs / result.numPixels < Base::_costStopThreshold ){
                break;
            }
        }

        result.pixelPercentage = ( float )result.numPixels / ( float )reference.dataSize( octave );
    }

    template <class WarpFunc, class LossFunc>
    inline void DLOptimizer<WarpFunc, LossFunc>::computeSteepestDescentStep( DeltaType& sdStep, const HessianType& h, const DeltaType& g ) const
    {
        sdStep = -g.squaredNorm() / ( g.transpose() * h * g ) * g;
    }

    template <class WarpFunc, class LossFunc>
    inline void DLOptimizer<WarpFunc, LossFunc>::computeGaussNewtonStep( DeltaType& gnStep, const HessianType& h, const DeltaType& g ) const
    {
        gnStep = -h.inverse() * g;
    }

    template <class WarpFunc, class LossFunc>
    inline void DLOptimizer<WarpFunc, LossFunc>::computeMixedStep( DeltaType& dlStep,
                                                                   const DeltaType& gnStep,
                                                                   const DeltaType& sdStep,
                                                                   float trustRadius ) const
    {
        DeltaType ds = gnStep - sdStep;

        // compute beta
        float a = ds.squaredNorm();
        float b = 2.0f * sdStep.dot( ds );
        float c = sdStep.squaredNorm() - Math::sqr( trustRadius );

        float s = Math::sqr( b ) - 4.0f * a * c;
        if( s < 0.0f ){
            dlStep = gnStep;
        } else {
            // beta = [-b +/- sqrt( b^2 - 4 a * c )] / 2 a
            float root = Math::sqrt( s );
            float beta1 = ( -b + root ) / ( 2.0f * a );
            float beta2 = ( -b - root ) / ( 2.0f * a );

            float beta = beta1;
            if( beta1 < 0.0f && beta2 > 0.0f ){
                beta = beta2;
            } else if ( beta1 > 0.0f && beta2 < 0.0f ){
                beta = beta1;
            } else {
                // take the better solution:
                float v1 = Math::abs( a * Math::sqr( beta1 ) + b * beta1 + c );
                float v2 = Math::abs( a * Math::sqr( beta2 ) + b * beta2 + c );

                std::cout << "beta1: " << beta1 << " beta2: " << beta2 << std::endl;

                beta = Math::min( v1, v2 );
            }
            dlStep = sdStep + beta * ds;
        }
    }

    template <class WarpFunc, class LossFunc>
    inline float DLOptimizer<WarpFunc, LossFunc>::updateTrustRadius( float radius, float fidelity, float tLow, float tHigh ) const
    {
        if( fidelity < tLow ){
            return _trDecreaseFactor * radius;
        } else if( fidelity > tHigh ){
            return _trIncreaseFactor * radius;
        }
        return radius;
    }


}

#endif // DLOptimizer_H
