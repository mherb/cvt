/*
   The MIT License (MIT)

   Copyright (c) 2011 - 2013, Philipp Heise and Sebastian Klose

   Permission is hereby granted, free of charge, to any person obtaining a copy
   of this software and associated documentation files (the "Software"), to deal
   in the Software without restriction, including without limitation the rights
   to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
   copies of the Software, and to permit persons to whom the Software is
   furnished to do so, subject to the following conditions:

   The above copyright notice and this permission notice shall be included in
   all copies or substantial portions of the Software.

   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
   OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
   THE SOFTWARE.
*/

#ifndef CVT_LMOPTIMIZER_H
#define CVT_LMOPTIMIZER_H

#include <cvt/vision/rgbdvo/Optimizer.h>

namespace cvt {

	template <class AlignData, class LossFunc>
	class LMOptimizer : public Optimizer<AlignData, LossFunc>
    {
        public:
            typedef LossFunc                                LossFuncType;
            LMOptimizer();
            ~LMOptimizer(){}

        private:
			typedef typename AlignData::WarpType			WarpFunc;
            typedef typename WarpFunc::JacobianType         JacobianType;
            typedef typename WarpFunc::HessianType          HessianType;
            typedef typename WarpFunc::DeltaVectorType      DeltaType;
			typedef RGBDKeyframe<AlignData>                 ReferenceType;
			typedef Optimizer<AlignData, LossFunc>          Base;
            typedef typename ReferenceType::JacobianVec     JacobianVec;
            typedef typename Base::Result                   ResultType;

            void optimizeSingleScale( ResultType& result,
                                      ReferenceType& reference,
                                      const Image& gray,
                                      const Image& depthImage,
                                      size_t octave );

            using Base::optimizeSingleScale;
    };

	template <class AlignData, class LossFunc>
	inline LMOptimizer<AlignData, LossFunc>::LMOptimizer() :
        Base()
    {
    }

	template <class AlignData, class LossFunc>
	inline void LMOptimizer<AlignData, LossFunc>::optimizeSingleScale( ResultType& result,
																	   ReferenceType& reference,
																	   const Image& gray,
																	   const Image& depthImage,
																	   size_t octave )
    {
        JacobianType deltaSum;
        HessianType  hessian;
        float lambda = 0.001f;

        IMapScoped<const float> grayMap( gray );
        IMapScoped<const float> depthMap( depthImage );

        result.iterations = 0;
        result.numPixels = 0;
        result.pixelPercentage = 0.0f;

        SIMD* simd = SIMD::instance();

        std::vector<float> residuals;
        JacobianVec jacobians;

        // initial costs
        reference.recompute( residuals, jacobians, result.warp, grayMap, depthMap, octave );
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
            for( size_t i = 0; i < hTmp.rows(); ++i )
                hTmp( i, i ) += ( lambda * Math::sqrt( hTmp( i, i ) ) );
            DeltaType deltaP = -hTmp.inverse() * deltaSum.transpose();

            if( deltaP.norm() < this->_minUpdate )
                break;
            result.warp.updateParameters( deltaP );

            // re-evaluate the cost function
            reference.recompute( residuals, jacobians, result.warp, grayMap, depthMap, octave );
            float currentCosts = simd->sumSqr( &residuals[ 0 ], residuals.size() );

            if( residuals.size() && currentCosts < result.costs ){
                // update the system:
                this->_overallDelta.noalias() += deltaP;
                result.costs = Base::evaluateSystem( hessian, deltaSum, &jacobians[ 0 ], &residuals[ 0 ], residuals.size() );
                savedWarp = result.warp;
                lambda *= 0.1f;
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
                lambda *= 2.0f;
            }
        }

        result.pixelPercentage = ( float )result.numPixels / ( float )reference.dataSize( octave );
    }

}


#endif // LMOPTIMIZER_H
