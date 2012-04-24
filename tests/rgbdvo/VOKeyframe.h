#ifndef CVT_VO_KEYFRAME_H
#define CVT_VO_KEYFRAME_H

#include <cvt/gfx/Image.h>
#include <cvt/math/Matrix.h>
#include <cvt/math/SE3.h>

#include <vector>
#include <Eigen/Core>
#include <KeyframeBase.h>

namespace cvt
{
    class VOKeyframe : public KeyframeBase<VOKeyframe>
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
            VOKeyframe( const Image& gray, const Image& depth, const Matrix4f& pose, const Matrix3f& K, const VOParams& params );

            ~VOKeyframe();

            const Matrix4f&     pose()                  const { return _pose; }

            /**
             *  \brief copmute the relative pose of an image w.r.t. this keyframe
             *  \param  predicted   input/output the pose of the image w.r.t. this keyframe
             *  \param  gray        the grayscale image of type GRAY_FLOAT
             *  \return Result information (ssd, iterations, numPixel, ...)
             */
            VOResult computeRelativePose( SE3<float>& predicted,
                                          const Image& gray,
                                          const Matrix3f& intrinsics,
                                          const VOParams& params ) const;

        protected:
            Matrix4f                    _pose;
            Image                       _gray;

            // the 3D points of this keyframe
            std::vector<Vector3f>       _points3d;

            // the pixel values (gray) for the points
            std::vector<float>          _pixelValues;

            // jacobians for that points
            std::vector<JacType>        _jacobians;

            // precomputed inverse of the hessian approx.
            Eigen::Matrix<float, 6, 6>  _inverseHessian;

            void computeJacobians( const Image& depth, const Matrix3f& intrinsics, const VOParams& params );
            void computeGradients( Image& gx, Image& gy ) const;
    };
}

#endif
