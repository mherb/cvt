/*
            CVT - Computer Vision Tools Library

 	 Copyright (c) 2012, Philipp Heise, Sebastian Klose

 	THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 	KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 	IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 	PARTICULAR PURPOSE.
*/

#ifndef CVT_ESMKEYFRAME_H
#define CVT_ESMKEYFRAME_H

#include <Eigen/Core>
#include <cvt/gfx/Image.h>
#include <cvt/math/Matrix.h>
#include <KeyframeBase.h>

namespace cvt {

    class ESMKeyframe : public KeyframeBase<ESMKeyframe>
    {
        public:			
            typedef Eigen::Matrix<float, 6, 6> HessianType;
            typedef Eigen::Matrix<float, 2, 6> JacType;

            /**
             * \param	gray            gray Image (float)
             * \param	depth           Depth Image (float)
             * \param	pose            pose for this keyframe
             * \param   K               the intrinsics for the rgb image
             * \param   dephtScaling    scale factor of the depth image: depthScaling equals to 1m!
             */
            ESMKeyframe( const Image& gray, const Image& depth, const Matrix4f& pose, const Matrix3f& K, const VOParams& params );
            ~ESMKeyframe();

            const JacType*      jacobians()             const { return &_jacobians[ 0 ]; }
            const Vector3f*     pointsPtr()             const { return &_points3d[ 0 ]; }
            const float*        pixelData()             const { return &_pixelValues[ 0 ]; }
            const Vector2f*     gradients()             const { return &_templateGradients[ 0 ]; }
            size_t              numPoints()             const { return _pixelValues.size(); }
            const Matrix4f&     pose()                  const { return _pose; }
            
            VOResult computeRelativePose( PoseRepresentation& predicted,
                                          const Image& gray,
                                          const Matrix3f& intrinsics,
                                          const VOParams& params ) const;

        protected:
            Matrix4f    _pose;
            Image       _gray;

            // the 3D points of this keyframe
            std::vector<Vector3f>   _points3d;

            // the image gradients of the used points
            std::vector<Vector2f>   _templateGradients;

            // the pixel values (gray) for the points
            std::vector<float>      _pixelValues;

            // jacobians for that points
            std::vector<JacType>    _jacobians;

            void computeJacobians( const Image& depth, const Matrix3f& intrinsics, const VOParams& params );
            void computeGradients( Image& gx, Image& gy ) const;
    
			float interpolatePixelValue( const Vector2f& pos, const float* ptr, size_t stride ) const;

			bool evalGradient( Vector2f& grad, const Vector2f* positions, const float* ptr, size_t stride, size_t w, size_t h ) const;
    };

}

#endif
