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

            void optimizeSingleScale( ResultType& result,
                                      RGBDKeyframe<WarpFunc>& reference,
                                      const Image& gray,
                                      const Image& /*depthImage*/,
                                      size_t octave ) const;
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
                                                                      size_t octave ) const
    {
        JacobianType deltaSum;
        HessianType  hessian;

        IMapScoped<const float> grayMap( gray );

        result.iterations = 0;
        result.numPixels = 0;
        result.pixelPercentage = 0.0f;

        LossFunc weighter( this->_robustThreshold );
        SystemBuilder<LossFunc> builder( weighter );


        std::vector<float> residuals;
        typename RGBDKeyframe<WarpFunc>::JacobianVec jacobians;

        while( result.iterations < this->_maxIter ){
            residuals.clear();
            jacobians.clear();

            // re-evaluate the cost function
            reference.recompute( residuals, jacobians, result.warp, grayMap, octave );

            float median = this->computeMedian( &residuals[ 0 ], residuals.size() );
            float mad = this->computeMAD( &residuals[ 0 ], residuals.size(), median );
            result.numPixels = residuals.size();

            // this is an estimate for the standard deviation:
            weighter.setScale( 1.4826f * mad );

            result.costs = builder.build( hessian,
                                              deltaSum,
                                              &jacobians[ 0 ],
                                              &residuals[ 0 ],
                                              residuals.size() );

            if( !result.numPixels /* no pixels projected */ ||
                result.costs / result.numPixels < 0.005f /* low cost threshold*/ ){
                break;
            }

            DeltaType deltaP = -hessian.inverse() * deltaSum.transpose();

            if( deltaP.norm() < this->_minUpdate )
                break;

            result.warp.updateParameters( deltaP );
            result.iterations++;
        }

        result.pixelPercentage = ( float )result.numPixels / ( float )reference.dataSize( octave );
    }
}

#endif // CVT_GNOPTIMIZER_H
