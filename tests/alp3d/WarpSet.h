#include <Eigen/Core>
#include <cvt/math/Matrix.h>

namespace cvt
{
    class WarpSet {
        public:
            /**
             * @brief WarpSet
             * @param n     number of initial random transforms
             */
            WarpSet( size_t n );

            /**
             * @brief size
             * @return number of current warp samples
             */
            size_t size() const { return _size; }
            size_t generate( size_t n );

            const PoseVec& warpParameter( size_t i ){ return _warps.block<6, 6>( 0, i ); }
            const Matrix4f& transform( size_t i ){ return _warpMats[ i ]; }

        private:
            // matrix of residual vectors
            typedef Eigen::Matrix<float, Eigen::Dynamic, 6> YMat;
            typedef Eigen::Matrix<float, 6, 1>              PoseVec;

            YMat                    _warps;
            std::vector<Matrix4f>   _warpMats;

    };
}
