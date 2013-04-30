/*
            CVT - Computer Vision Tools Library

     Copyright (c) 2012, Philipp Heise, Sebastian Klose

    THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
    KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
    IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
    PARTICULAR PURPOSE.
*/

#ifndef CVT_GNOPTIMIZER_H
#define CVT_GNOPTIMIZER_H

#include <cvt/vision/rgbdvo/Optimizer.h>

#include <Eigen/LU>

namespace cvt {

    template <class WarpFunc, class LossFunc>
    class GNOptimizer : public Optimizer<WarpFunc, LossFunc>
    {
        public:
            GNOptimizer();
            virtual ~GNOptimizer(){}

        protected:
            typedef typename WarpFunc::JacobianType     JacobianType;
            typedef typename WarpFunc::HessianType      HessianType;
            typedef typename WarpFunc::DeltaVectorType  DeltaType;
            typedef typename Optimizer<WarpFunc, LossFunc>::Result ResultType;
            typedef typename Optimizer<WarpFunc, LossFunc>::AlignDataType AlignDataType;
            typedef RGBDKeyframe<WarpFunc>              KFType;

            void optimizeSingleScale( ResultType& result,
                                      RGBDKeyframe<WarpFunc>& reference,
                                      const Image& gray,
                                      const Image& /*depthImage*/,
                                      size_t octave );

            void optimizeSingleScale( ResultType& result,
                                      KFType* references, size_t nRefs,
                                      const Image& gray,
                                      const Image& /*depthImage*/,
                                      size_t octave );
    };

    template <class WarpFunc, class LossFunc>
    inline GNOptimizer<WarpFunc, LossFunc>::GNOptimizer() :
        Optimizer<WarpFunc, LossFunc>()
    {
    }

    template <class WarpFunc, class LossFunc>
    inline void GNOptimizer<WarpFunc, LossFunc>::optimizeSingleScale( ResultType& result,
                                                                      RGBDKeyframe<WarpFunc>& reference,
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

        std::vector<float> residuals;
        typename RGBDKeyframe<WarpFunc>::JacobianVec jacobians;

        while( result.iterations < this->_maxIter ){
            residuals.clear();
            jacobians.clear();

            // re-evaluate the cost function
            reference.recompute( residuals, jacobians, result.warp, grayMap, octave );

            result.numPixels = residuals.size();
            result.costs = this->evaluateSystem( hessian,
                                                 deltaSum,
                                                 &jacobians[ 0 ],
                                                 &residuals[ 0 ],
                                                 residuals.size() );
            if( this->_logError ){
                this->_logger.log( octave, result.iterations, result.warp.pose() );
            }

            if( !result.numPixels /* no pixels projected */ ||
                result.costs / result.numPixels < this->_costStopThreshold ){
                break;
            }

            DeltaType deltaP = -hessian.inverse() * deltaSum.transpose();
            this->_overallDelta.noalias() += deltaP;

            if( deltaP.norm() < this->_minUpdate )
                break;

            result.warp.updateParameters( deltaP );
            result.iterations++;
        }

        result.pixelPercentage = ( float )result.numPixels / ( float )reference.dataSize( octave );
    }

    template <class WarpFunc, class LossFunc>
    inline void GNOptimizer<WarpFunc, LossFunc>::optimizeSingleScale( ResultType& result,
                                                                      KFType* references, size_t nRefs,
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

        std::vector<float> residuals, resTmp;
        typename RGBDKeyframe<WarpFunc>::JacobianVec jacobians, jacTmp;

        size_t overallPixels = 0;

        while( result.iterations < this->_maxIter ){
            residuals.clear();
            jacobians.clear();

            // re-evaluate the cost functions
            overallPixels = 0;
            for( size_t r = 0; r < nRefs; r++ ){
                resTmp.clear();
                jacTmp.clear();
                references[ r ].recompute( resTmp, jacTmp, result.warp, grayMap, octave );

                residuals.insert( residuals.begin() + residuals.size(), resTmp.begin(), resTmp.end() );
                jacobians.insert( jacobians.begin() + jacobians.size(), jacTmp.begin(), jacTmp.end() );
                overallPixels += references[ r ].dataSize( octave );
            }

            result.numPixels = residuals.size();
            result.costs = this->evaluateSystem( hessian,
                                                 deltaSum,
                                                 &jacobians[ 0 ],
                                                 &residuals[ 0 ],
                                                 residuals.size() );
            if( this->_logError ){
                this->_logger.log( octave, result.iterations, result.warp.pose() );
            }

            if( !result.numPixels /* no pixels projected */ ||
                result.costs / result.numPixels < this->_costStopThreshold ){
                break;
            }

            DeltaType deltaP = -hessian.inverse() * deltaSum.transpose();
            this->_overallDelta.noalias() += deltaP;

            if( deltaP.norm() < this->_minUpdate )
                break;

            result.warp.updateParameters( deltaP );
            result.iterations++;
        }

        result.pixelPercentage = ( float )result.numPixels / ( float )overallPixels;
    }
}

#endif // CVT_GNOPTIMIZER_H
