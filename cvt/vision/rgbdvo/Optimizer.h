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

#include <Eigen/LU>

namespace cvt {

    template <class WarpFunc, class Weighter>
    class Optimizer
    {
        public:
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
            void setRobustThreshold( float v )      { _robustThreshold = v; }
            void setMinPixelPercentage( float v )   { _minPixelPercentage = v; }

            // scale-space optimization
            void optimize( Result& result,
                           const Matrix4f& posePrediction,
                           RGBDKeyframe<WarpFunc>& reference,
                           const ImagePyramid& grayPyramid,
                           const Image& depthImage ) const;


        protected:
            typedef typename WarpFunc::JacobianType     JacobianType;
            typedef typename WarpFunc::HessianType      HessianType;
            typedef typename WarpFunc::DeltaVectorType  DeltaType;
            typedef typename RGBDKeyframe<WarpFunc>::AlignmentData AlignDataType;
            size_t  _maxIter;
            float   _minUpdate;
            float   _robustThreshold;
            float   _minPixelPercentage;

            virtual void optimizeSingleScale( Result& result,
                                              RGBDKeyframe<WarpFunc>& reference,
                                              const Image& gray,
                                              const Image& depthImage,
                                              size_t octave ) const = 0;

            float computeMedian( const float* residuals, size_t n ) const;
            float computeMAD( const float* residuals, size_t n, float median ) const;
            bool checkResult( const Result& res ) const;

    };

    template <class WarpFunc, class LossFunc>
    inline Optimizer<WarpFunc, LossFunc>::Optimizer() :
        _maxIter( 10 ),
        _minUpdate( 1e-6 ),
        _robustThreshold( 0.1f ),
        _minPixelPercentage( 0.8f )
    {
    }

    template <class WarpFunc, class LossFunc>
    inline void Optimizer<WarpFunc, LossFunc>::optimize( Result& result,
                                                         const Matrix4f& posePrediction,
                                                         RGBDKeyframe<WarpFunc> &reference,
                                                         const ImagePyramid& grayPyramid,
                                                         const Image& depthImage ) const
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

}

#endif // OPTIMIZER_H
