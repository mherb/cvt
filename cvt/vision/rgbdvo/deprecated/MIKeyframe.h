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

#ifndef CVT_MI_KEYFRAME_H
#define CVT_MI_KEYFRAME_H

#include <cvt/gfx/Image.h>
#include <cvt/math/Matrix.h>
#include <cvt/math/SE3.h>

#include <vector>
#include <Eigen/Core>
#include <cvt/vision/rgbdvo/KeyframeBase.h>
#include <Eigen/StdVector>

namespace cvt
{
    class MIKeyframe : public KeyframeBase<MIKeyframe>
    {
        public:
            typedef Eigen::Matrix<float, 6, 6> HessianType;
            typedef Eigen::Matrix<float, 1, 6> JacType;

            /**
             * \param	gray            gray Image (float)
             * \param	depth           Depth Image (float)
             * \param	pose            pose for this keyframe
             * \param   K               the intrinsics for the rgb image
             * \param   params          parameters
             */
            MIKeyframe( const Image& gray, const Image& depth, const Matrix4f& pose, const Matrix3f& K, const VOParams& params );

            ~MIKeyframe();

            const Matrix4f&     pose()                  const { return _pose; }

            /**
             *  \brief compute the relative pose of an image w.r.t. this keyframe
             *  \param  predicted   input/output the pose of the image w.r.t. this keyframe
             *  \param  gray        the grayscale image of type GRAY_FLOAT
             *  \return Result information (ssd, iterations, numPixel, ...)
             */
            VOResult computeRelativePose( PoseRepresentation& predicted,
                                          const Image& gray,
                                          const Matrix3f& intrinsics,
                                          const VOParams& params );

        protected:
            static const float          LogOffset = 1e-4f;
            IKernel                     _kx, _ky, _kxx, _kyy;
            Matrix4f                    _pose;
            Image                       _gray;

            /* Mutual Information stuff */
            size_t						_numBins;
            float*						_jointHistogram;
            float*						_templateHistogram;
            float*						_logFactors;

            // the 3D points of this keyframe
            std::vector<Vector3f>       _points3d;

            // the pixel values (gray) for the points
            std::vector<float>          _pixelValues;
            std::vector<int>            _templateBins;

            // jacobians for that points
            typedef std::vector<HessianType, Eigen::aligned_allocator<HessianType> > HessianVector;
            // screen jacobians and screen hessians of image w.r.t. pose!
            std::vector<JacType>        _jacobians;
            HessianVector               _hessians;

            // the mutual information heassian (stored as inverse for efficiency)
            HessianType                 _hessian;

            std::vector<Vector4f>       _splineWeights;
            std::vector<Vector4f>       _splineDerivativeWeights;
            std::vector<Vector4f>       _splineSecondDerivativeWeights;

            void computeJacobians( const Image& depth, const Matrix3f& intrinsics, const VOParams& params );

            void addToHistograms( int pixVal, const Vector4f& weights );

            void evaluateApproximateHessian();

            void clearJointHistogram( const SIMD* simd );

            void addToJointHistogram( int r, int t, const Vector4f& rvals, const Vector4f& tvals );

            void updateLogFactors();

            float calcSummedFactors( int r, int t, const Vector4f& rVals, const Vector4f& tVals );

            float MI();

            void checkJointHistogram();
            void checkTempHistogram();
            void checkJacobianComputation();

    };
}

#endif
