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
                                              size_t octave ) const;

            float computeMedian( const float* residuals, size_t n ) const
            {
                ApproxMedian medianSelector( 0.0f, 1.0f, 0.02f );

                for( size_t i = 0; i < n; ++i ){
                    medianSelector.add( Math::abs( residuals[ i ] ) );
                }

                return medianSelector.approximateNth( n >> 1 );
            }

            float computeMAD( const float* residuals, size_t n, float median ) const
            {
                ApproxMedian medianSelector( 0.0f, 0.5f, 0.02f );

                for( size_t i = 0; i < n; ++i ){
                    medianSelector.add( Math::abs( residuals[ i ] - median ) );
                }

                return medianSelector.approximateNth( n >> 1 );
            }

            void validIndices( std::vector<size_t>& indices, const float* vals, size_t num, float minVal ) const
            {
                indices.clear();
                indices.reserve( num );

                for( size_t i = 0; i < num; i++ ){
                    if( vals[ i ] > minVal ){
                        indices.push_back( i );
                    }
                }
            }

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
    inline void Optimizer<WarpFunc, LossFunc>::optimizeSingleScale( Result& result,
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

        LossFunc weighter( _robustThreshold );
        SystemBuilder<LossFunc> builder( weighter );


        std::vector<float> residuals;
        typename RGBDKeyframe<WarpFunc>::JacobianVec jacobians;

        while( result.iterations < _maxIter ){
            residuals.clear();
            jacobians.clear();

            // re-evaluate the cost function
            reference.recompute( residuals, jacobians, result.warp, grayMap, octave );

            float median = computeMedian( &residuals[ 0 ], residuals.size() );
            float mad = computeMAD( &residuals[ 0 ], residuals.size(), median );
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

            if( deltaP.norm() < _minUpdate )
                break;

            result.warp.updateParameters( deltaP );
            result.iterations++;
        }

        result.pixelPercentage = ( float )result.numPixels / ( float )reference.dataSize( octave );
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

        // at least on one scale it should work
        bool resultOk = false;
        Result saveResult = result;

        reference.updateOnlineData( grayPyramid, dephtImage );
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
