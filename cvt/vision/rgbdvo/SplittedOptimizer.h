/*
            CVT - Computer Vision Tools Library

     Copyright (c) 2012, Philipp Heise, Sebastian Klose

    THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
    KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
    IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
    PARTICULAR PURPOSE.
*/

#ifndef SPLITTED_OPTIMIZER_H
#define SPLITTED_OPTIMIZER_H

#include <cvt/vision/rgbdvo/Optimizer.h>

namespace cvt {

    template <class WarpFunc, class Weighter>
    class SplittedOptimizer : public Optimizer<WarpFunc, Weighter>
    {
        public:
            typedef typename RGBDKeyframe<WarpFunc>::Result   ResultType;

            SplittedOptimizer();
            virtual ~SplittedOptimizer(){}

            void optimize( ResultType& result,
                           const Matrix4f& posePrediction,
                           RGBDKeyframe<WarpFunc>& reference,
                           const ImagePyramid& grayPyramid,
                           const Image& depthImage ) const;


        protected:
            typedef Optimizer<WarpFunc, Weighter>   Base;
            typedef typename Base::JacobianType     JacobianType;
            typedef typename Base::HessianType      HessianType;
            typedef typename Base::DeltaType        DeltaType;
            typedef typename RGBDKeyframe<WarpFunc>::AlignDataType AlignDataType;
    };

    template <class WarpFunc, class LossFunc>
    inline SplittedOptimizer<WarpFunc, LossFunc>::SplittedOptimizer()
    {
    }

    template <class WarpFunc, class LossFunc>
    inline void SplittedOptimizer<WarpFunc, LossFunc>::optimize( ResultType& result,
                                                                 const Matrix4f& posePrediction,
                                                                 RGBDKeyframe<WarpFunc> &reference,
                                                                 const ImagePyramid& grayPyramid,
                                                                 const Image& /*depthImage*/ ) const
    {
        SIMD* simd = SIMD::instance();
        Matrix4f tmp4;
        tmp4 = posePrediction.inverse() * reference.pose();

        result.warp.setPose( tmp4 );
        result.costs = 0.0f;
        result.iterationsOnOctave.resize( grayPyramid.octaves(), 0 );
        result.numPixels = 0;
        result.pixelPercentage = 0.0f;

        LossFunc weighter( Base::_robustThreshold );
        SystemBuilder<LossFunc> builder( weighter );

        Matrix4f projMat;
        std::vector<Vector2f> warpedPts;
        std::vector<float> interpolatedPixels;
        std::vector<float> residuals;
        // sum of jacobians * delta
        JacobianType deltaSum;
        HessianType  hessian;

        for( int o = grayPyramid.octaves() - 1; o >= 0; o-- ){
            ResultType scaleResult;
            scaleResult = result;

            const size_t width = grayPyramid[ o ].width();
            const size_t height = grayPyramid[ o ].height();

            const AlignDataType& data = reference.dataForScale( o );

            const size_t num = data.points3d.size();
            Matrix4f K4( data.intrinsics );
            const Vector3f* p3dPtr = &data.points3d[ 0 ];
            const float* referencePixVals = &data.pixelValues[ 0 ];
            const JacobianType* referenceJ = &data.jacobians[ 0 ];

            warpedPts.resize( num );
            interpolatedPixels.resize( num );
            residuals.resize( num );

            IMapScoped<const float> grayMap( grayPyramid[ o ] );

            scaleResult.iterationsOnOctave[ o ] = 0;
            scaleResult.numPixels = 0;
            scaleResult.pixelPercentage = 0.0f;

            bool rotationOnly = true;
            bool translationOnly = false;
            size_t alternatingSteps = 6;

            //Base::_maxIter += ( rotoOpts + transOpts );

            std::vector<size_t> indices;
            while( scaleResult.iterationsOnOctave[ 0 ] < Base::_maxIter ){
                // build the updated projection Matrix
                projMat = K4 * scaleResult.warp.poseMatrix();

                // project the points:
                simd->projectPoints( &warpedPts[ 0 ], projMat, p3dPtr, num );

                // interpolate the pixel values
                simd->warpBilinear1f( &interpolatedPixels[ 0 ], &warpedPts[ 0 ].x, grayMap.ptr(), grayMap.stride(), width, height, -1.0f, num );
                scaleResult.warp.computeResiduals( &residuals[ 0 ], referencePixVals, &interpolatedPixels[ 0 ], num );

                this->validIndices( indices, &interpolatedPixels[ 0 ], num, -0.01f );

                float median = Base::computeMedian( &residuals[ 0 ], num, indices );
                weighter.setSigma( 1.4f * median ); /* this is an estimate for the standard deviation */

                /* a hack: the builder does not touch the hessian if its a non robust lossfunc!*/
                hessian = data.hessian;
                scaleResult.numPixels = builder.build( hessian, deltaSum,
                                                       referenceJ,
                                                       &residuals[ 0 ],
                                                       indices,
                                                       scaleResult.costs );
                if( !scaleResult.numPixels ){
                    break;
                }

                scaleResult.iterationsOnOctave[ o ]++;

                if( translationOnly ){
                    Eigen::Matrix<float, 3, 3> h3;
                    Eigen::Matrix<float, 3, 1> d3, r3;
                    // block corresponding to translation
                    h3 = hessian.template block<3, 3>( 3, 3 );
                    r3 = deltaSum.transpose().template block<3, 1>( 3, 0 );
                    d3 = -h3.inverse() * r3;

                    DeltaType deltaP = DeltaType::Zero();
                    deltaP.template tail<3>() = d3;
                    scaleResult.warp.updateParameters( deltaP );

                    translationOnly = false;
                    alternatingSteps--;
                    if( alternatingSteps )
                        rotationOnly = true;
                    continue;
                }

                if( rotationOnly ){
                    Eigen::Matrix<float, 3, 3> h3;
                    Eigen::Matrix<float, 3, 1> d3, r3;
                    // block corresponding to rotation
                    h3 = hessian.template block<3, 3>( 0, 0 );
                    r3 = deltaSum.transpose().template block<3, 1>( 0, 0 );
                    d3 = -h3.inverse() * r3;

                    DeltaType deltaP = DeltaType::Zero();
                    deltaP.template head<3>() = d3;
                    scaleResult.warp.updateParameters( deltaP );

                    rotationOnly = false;
                    alternatingSteps--;
                    if( alternatingSteps )
                        translationOnly = true;

                    continue;
                }

                // normal loop: optimizing all
                // evaluate the delta parameters
                DeltaType deltaP = -hessian.inverse() * deltaSum.transpose();
                scaleResult.warp.updateParameters( deltaP );

                if( deltaP.norm() < Base::_minUpdate )
                    break;

            }

            if( scaleResult.numPixels )
                scaleResult.pixelPercentage = ( float )scaleResult.numPixels / ( float )num;

            // TODO: ensure the result on this scale is good enough (pixel percentage & error )
            result = scaleResult;
        }

        tmp4 = result.warp.poseMatrix();
        tmp4 = reference.pose() * tmp4.inverse();
        result.warp.setPose( tmp4 );
    }

}

#endif // OPTIMIZER_H
