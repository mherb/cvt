#ifndef CVT_VO_KEYFRAME_H
#define CVT_VO_KEYFRAME_H

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
                                          const VOParams& params ) const;

        protected:
            IKernel                     _kx, _ky, _kxx, _kyy;
            Matrix4f                    _pose;
            Image                       _gray;

            /* Mutual Information stuff */
            size_t						_numBins;
            float*						_jointHistogram;
            float*						_templateHistogram;

            // the 3D points of this keyframe
            std::vector<Vector3f>       _points3d;

            // the pixel values (gray) for the points
            std::vector<float>          _pixelValues;

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
    };
}

#endif
