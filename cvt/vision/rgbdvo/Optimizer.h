/*
            CVT - Computer Vision Tools Library

     Copyright (c) 2012, Philipp Heise, Sebastian Klose

    THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
    KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
    IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
    PARTICULAR PURPOSE.
*/

#ifndef OPTIMIZER_H
#define OPTIMIZER_H

#include <cvt/vision/rgbdvo/RGBDKeyframe.h>
#include <cvt/vision/rgbdvo/SystemBuilder.h>
#include <cvt/vision/rgbdvo/ApproxMedian.h>
#include <Eigen/LU>

namespace cvt {

    template <class WarpFunc, class Weighter>
    class Optimizer
    {
        public:
            typedef typename WarpFunc::JacobianType     JacobianType;
            typedef typename WarpFunc::HessianType      HessianType;
            typedef typename WarpFunc::DeltaVectorType  DeltaType;
            typedef typename RGBDKeyframe<WarpFunc>::AlignmentData AlignDataType;

            struct Result {
                EIGEN_MAKE_ALIGNED_OPERATOR_NEW
                Result() :
                    success( false ),
                    numPixels( 0 ),
                    pixelPercentage( 0.0f ),
                    costs( 0.0f )
                {
                }

                bool        success;
                size_t      iterations;
                size_t      numPixels;
                float       pixelPercentage; /* between 0 and 1 */
                float       costs;

                WarpFunc    warp;
            };

            Optimizer();
            virtual ~Optimizer(){}

            void setMaxIterations( size_t iter )    { _maxIter = iter; }
            void setMinUpdate( float v )            { _minUpdate = v; }
            //void setRobustThreshold( float v )      { _weighter.setThreshold( v ); }
            void setMinPixelPercentage( float v )   { _minPixelPercentage = v; }
            void setUseRegularization( bool v )     { _useRegularizer = v; }
            void setRegularizationMatrix( const HessianType& m ) { _regularizer = m; }

            /**
             * @brief setCostStopThreshold
             * @param v early stepout, if average costs are below v!
             */
            void setCostStopThreshold( float v )    { _costStopThreshold = v; }

            // scale-space optimization
            void optimize( Result& result,
                           const Matrix4f& posePrediction,
                           RGBDKeyframe<WarpFunc>& reference,
                           const ImagePyramid& grayPyramid,
                           const Image& depthImage );

        protected:
            size_t          _maxIter;
            float           _minUpdate;
            float           _minPixelPercentage;
            float           _costStopThreshold;
            bool            _useRegularizer;
            HessianType     _regularizer;
            DeltaType       _overallDelta;

            Weighter                _weighter;
            SystemBuilder<Weighter> _builder;

            float computeMedian( const float* residuals, size_t n ) const;
            float computeMAD( const float* residuals, size_t n, float median ) const;
            bool checkResult( const Result& res ) const;

            float evaluateSystem( HessianType& hessian, JacobianType& deltaSum,
                                  const JacobianType* jacobians, const float* residuals, size_t n );

            void resetOverallDelta();

        private:
            virtual void optimizeSingleScale( Result& result,
                                              RGBDKeyframe<WarpFunc>& reference,
                                              const Image& gray,
                                              const Image& depthImage,
                                              size_t octave ) = 0;

    };

    template <class WarpFunc, class LossFunc>
    inline Optimizer<WarpFunc, LossFunc>::Optimizer() :
        _maxIter( 10 ),
        _minUpdate( 1e-6 ),
        _minPixelPercentage( 0.8f ),
        _costStopThreshold( 0.005f ),
        _useRegularizer( false ),
        _regularizer( HessianType::Zero() ),
        _overallDelta( DeltaType::Zero() ),
        _builder( _weighter )
    {
    }

    template <class WarpFunc, class LossFunc>
    inline void Optimizer<WarpFunc, LossFunc>::optimize( Result& result,
                                                         const Matrix4f& posePrediction,
                                                         RGBDKeyframe<WarpFunc> &reference,
                                                         const ImagePyramid& grayPyramid,
                                                         const Image& depthImage )
    {
        Matrix4f tmp4;
        tmp4 = posePrediction.inverse() * reference.pose();

        result.warp.setPose( tmp4 );
        result.costs = 0.0f;
        result.iterations = 0;
        result.numPixels = 0;
        result.pixelPercentage = 0.0f;

//      this is for computing the ESM jacobians
//        ImagePyramid gradX( grayPyramid.octaves(), grayPyramid.scaleFactor() );
//        ImagePyramid gradY( grayPyramid.octaves(), grayPyramid.scaleFactor() );
//        grayPyramid.convolve( gradX, reference.kernelDx() );
//        grayPyramid.convolve( gradY, reference.kernelDy());

        // ensure that we had a successful optimization
        bool resultOk = false;
        Result saveResult = result;

        if( _useRegularizer ){
            resetOverallDelta();
        }

        reference.updateOnlineData( grayPyramid, depthImage );
        for( int o = grayPyramid.octaves() - 1; o >= 0; o-- ){
            this->optimizeSingleScale( result, reference, grayPyramid[ o ], depthImage, o );

            if( checkResult( result ) ){
                saveResult = result;
                resultOk = true;
            }
        }

        result = saveResult;

        // convert relative result to absolute pose: TODO: maybe do this outside
        tmp4 = result.warp.pose();
        tmp4 = reference.pose() * tmp4.inverse();
        result.warp.setPose( tmp4 );
    }

    template <class WarpFunc, class LossFunc>
    inline void Optimizer<WarpFunc, LossFunc>::resetOverallDelta()
    {
        _overallDelta.setZero();
    }

    template <class WarpFunc, class LossFunc>
    inline float Optimizer<WarpFunc, LossFunc>::computeMedian( const float* residuals, size_t n ) const
    {
        ApproxMedian medianSelector( 0.0f, 1.0f, 0.02f );

        for( size_t i = 0; i < n; ++i ){
            medianSelector.add( Math::abs( residuals[ i ] ) );
        }

        return medianSelector.approximateNth( n >> 1 );
    }

    template <class WarpFunc, class LossFunc>
    inline float Optimizer<WarpFunc, LossFunc>::computeMAD( const float* residuals, size_t n, float median ) const
    {
        ApproxMedian medianSelector( 0.0f, 0.5f, 0.02f );

        for( size_t i = 0; i < n; ++i ){
            medianSelector.add( Math::abs( residuals[ i ] - median ) );
        }

        return medianSelector.approximateNth( n >> 1 );
    }

    template <class WarpFunc, class LossFunc>
    inline bool Optimizer<WarpFunc, LossFunc>::checkResult( const Result& res ) const
    {
        // too few pixels projected into image
        if( res.pixelPercentage < _minPixelPercentage ){
            //std::cout << "Pixel Percentage: " << res.pixelPercentage << " : " << _minPixelPercentage << std::endl;
            return false;
        }
        return true;
    }


    template <class WarpFunc, class LossFunc>
    inline float Optimizer<WarpFunc, LossFunc>::evaluateSystem( HessianType& hessian, JacobianType& deltaSum,
                                                                const JacobianType* jacobians, const float* residuals, size_t n  )
    {
        float median = this->computeMedian( residuals, n );
        float mad = this->computeMAD( residuals, n, median );

        // this is an estimate for the standard deviation:
        _weighter.setScale( 1.4826f * mad );
        float costs = _builder.build( hessian,
                                      deltaSum,
                                      jacobians,
                                      residuals,
                                      n );
        if( _useRegularizer ){
            hessian.noalias()  += _regularizer;
            deltaSum.noalias() += _regularizer * _overallDelta;
        }

        return costs;
    }

}

#endif // OPTIMIZER_H
