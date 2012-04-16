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
            /**
             * @param	rgb		RGB Image
             * @param	depth	Depth Image
             * @param	pose	pose for this keyframe
             */
            VOKeyframe( const Image& rgb, const Image& depth, const Matrix4f& pose );
            ~VOKeyframe();

            const HessianType& inverseHessian() const { return _inverseHessian; }

        private:
            Matrix4f    _pose;

            Image       _gray;
            Image       _depth;

            // jacobians per pixel mixed with the image gradients
            std::vector<Image>  _stepestDescentImages;

            // precomputed inverse of the hessian approx.
            Eigen::Matrix<float, 6, 6>  _inverseHessian;
    };

    inline VOKeyframe::VOKeyframe( const Image& rgb, const Image& depth, const Matrix4f& pose ) :
        _pose( pose ),
        _depth( depth )
    {
        rgb.convert( _gray, IFormat::GRAY_UINT8 );

        // todo: compute the gradients & build the jacobian images
        //       for the jacobians, we also need the intrinsics of the rgb cam
    }

    inline VOKeyframe::~VOKeyframe()
    {
    }
}

#endif
