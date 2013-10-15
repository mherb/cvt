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


#ifndef CVT_GNOPTIMIZER_H
#define CVT_GNOPTIMIZER_H

#include <cvt/vision/rgbdvo/Optimizer.h>

#include <Eigen/LU>

namespace cvt {

	template <class AlignData, class LossFunc>
	class GNOptimizer : public Optimizer<AlignData, LossFunc>
    {
        public:
            GNOptimizer();
            virtual ~GNOptimizer(){}

        protected:
			typedef typename AlignData::WarpType		WarpFunc;
            typedef typename WarpFunc::JacobianType     JacobianType;
            typedef typename WarpFunc::HessianType      HessianType;
            typedef typename WarpFunc::DeltaVectorType  DeltaType;
			typedef typename Optimizer<AlignData, LossFunc>::Result ResultType;
			typedef RGBDKeyframe<AlignData>              KFType;

            void optimizeSingleScale( ResultType& result,
									  RGBDKeyframe<AlignData>& reference,
                                      const Image& gray,
                                      const Image& depthImage,
                                      size_t octave );

            void optimizeSingleScale( ResultType& result,
                                      KFType* references, size_t nRefs,
                                      const Image& gray,
                                      const Image& depthImage,
                                      size_t octave );
    };

	template <class AlignData, class LossFunc>
	inline GNOptimizer<AlignData, LossFunc>::GNOptimizer() :
		Optimizer<AlignData, LossFunc>()
    {
    }

	template <class AlignData, class LossFunc>
	inline void GNOptimizer<AlignData, LossFunc>::optimizeSingleScale( ResultType& result,
																	   RGBDKeyframe<AlignData>& reference,
																	   const Image& gray,
																	   const Image& depthImage,
																	   size_t octave )
    {
        JacobianType deltaSum;
        HessianType  hessian;

        IMapScoped<const float> grayMap( gray );
        IMapScoped<const float> depthMap( depthImage );

        result.iterations = 0;
        result.numPixels = 0;
        result.pixelPercentage = 0.0f;

        std::vector<float> residuals;
		typename RGBDKeyframe<AlignData>::JacobianVec jacobians;

        while( result.iterations < this->_maxIter ){
            residuals.clear();
            jacobians.clear();

            // re-evaluate the cost function
            reference.recompute( residuals, jacobians, result.warp, grayMap, depthMap, octave );

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

	template <class AlignData, class LossFunc>
	inline void GNOptimizer<AlignData, LossFunc>::optimizeSingleScale( ResultType& result,
																	   KFType* references, size_t nRefs,
																	   const Image& gray,
																	   const Image& depthImage,
																	   size_t octave )
    {
        JacobianType deltaSum;
        HessianType  hessian;

        IMapScoped<const float> grayMap( gray );
        IMapScoped<const float> depthMap( depthImage );

        result.iterations = 0;
        result.numPixels = 0;
        result.pixelPercentage = 0.0f;

        std::vector<float> residuals, resTmp;
		typename RGBDKeyframe<AlignData>::JacobianVec jacobians, jacTmp;

        size_t overallPixels = 0;

        while( result.iterations < this->_maxIter ){
            residuals.clear();
            jacobians.clear();

            // re-evaluate the cost functions
            overallPixels = 0;
            for( size_t r = 0; r < nRefs; r++ ){
                resTmp.clear();
                jacTmp.clear();
                references[ r ].recompute( resTmp, jacTmp, result.warp, grayMap, depthMap, octave );

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
