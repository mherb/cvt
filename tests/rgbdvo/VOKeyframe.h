#ifndef CVT_VO_KEYFRAME_H
#define CVT_VO_KEYFRAME_H

#include <cvt/gfx/Image.h>
#include <cvt/math/Matrix.h>

#include <vector>
#include <Eigen/Core>

namespace cvt
{
    class VOKeyframe
    {
        public:
            typedef Eigen::Matrix<float, 6, 6> HessianType;
            typedef Eigen::Matrix<float, 1, 6> JacType;

            /**
             * \param	gray            gray Image (float)
             * \param	depth           Depth Image (float)
             * \param	pose            pose for this keyframe
             * \param   K               the intrinsics for the rgb image
             * \param   dephtScaling    scale factor of the depth image: depthScaling equals to 1m!
             */
            VOKeyframe( const Image& gray, const Image& depth, const Matrix4f& pose, const Matrix3f& K, float depthScaling = 5000.0f );
            ~VOKeyframe();

            const HessianType&  inverseHessian()        const { return _inverseHessian; }
            const JacType*      jacobians()             const { return &_jacobians[ 0 ]; }
            const Vector3f*     pointsPtr()             const { return &_points3d[ 0 ]; }
            const float*        pixelData()             const { return &_pixelValues[ 0 ]; }
            size_t              numPoints()             const { return _points3d.size(); }
            const Matrix4f&     pose()                  const { return _pose; }

        private:
            Matrix4f    _pose;
            Image       _gray;

            // the 3D points of this keyframe
            std::vector<Vector3f>   _points3d;

            // the image positions of the used points
            std::vector<Vector2f>   _pixelPositions;

            // the pixel values (gray) for the points
            std::vector<float>      _pixelValues;

            // jacobians for that points
            std::vector<JacType>    _jacobians;

            // precomputed inverse of the hessian approx.
            Eigen::Matrix<float, 6, 6>  _inverseHessian;

            void computeJacobians( const Image& depth, const Matrix3f& intrinsics, float invDepthScale );

            void computeGradients( Image& gx, Image& gy ) const;
    };
}

#endif
