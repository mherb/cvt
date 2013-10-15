#ifndef CVT_LMOPTIMIZER_H
#define CVT_LMOPTIMIZER_H

#include <cvt/vision/rgbdvo/Optimizer.h>

namespace cvt {

	template <class AlignData, class LossFunc>
	class LMOptimizer : public Optimizer<AlignData, LossFunc>
    {
        public:
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
                lambda *= 10.0f;
            }
        }

        result.pixelPercentage = ( float )result.numPixels / ( float )reference.dataSize( octave );
    }

}


#endif // LMOPTIMIZER_H
