#include <Eigen/Core>
#include <cvt/math/Vector.h>
#include <cvt/gfx/Image.h>
#include <cvt/gfx/IMapScoped.h>

namespace cvt {

    /* this is the class holding the current overall predictor*/
    class ALP3D {
        public:
            ALP3D( const Matrix3f& intrinsics );
            ~ALP3D();

            /**
             * @brief size
             * @return current number of points in the predictor
             */
            size_t size() const;

            /* add new 3d points */
            void learn( const std::vector<Vector3f>& samplePoints,
                        const std::vector<float>& values,
                        const Image& gray );

            /**
             * @brief track track the pose of the camera using the current predictors
             * @param pose
             * @param gray
             */
            void track( Matrix4f& pose, const Image& gray );

        private:
            // predictor matrix
            typedef Eigen::Matrix<float, 6, Eigen::Dynamic>              LPMat;
            typedef Eigen::Matrix<float, 6, 1>                           PoseVec;
            // matrix of pose vector
            typedef Eigen::Matrix<float, 6, Eigen::Dynamic>              YMat;
            typedef Eigen::Matrix<float, Eigen::Dynamic, Eigen::Dynamic> HMat;

            Matrix4f                _intrinsics;

            // predictor matrix for these points
            LPMat                   _predictor;

            // matrix with all warp parameters
            YMat                    _warps;
            std::vector<Matrix4f>   _warpMats;

            std::vector<Vector3f>   _currentPoints3d;
            std::vector<Vector2f>   _parameterRanges;
            std::vector<float>      _intensities;

            std::vector<Vector2f>   _lastWarped;
            SIMD*                   _simd;

            void updatePoseVector()
            {
                // generate n new samples in the warpSet
                // compute the necessary update to the predictor
            }

            size_t numTransformSamples() const { return _warpMats.size(); }

            void demeanNormalize(float *v , size_t n) const;
            void computeDifference( float* normalizedWarped,
                                    const float* sampleValues,
                                    size_t n ) const;

            void generateInitialRandomWarps( size_t n );
            void genRandomPoseVec( float* column );

            void computeIntensities( float* intensities,
                                     std::vector<Vector2f>& warped,
                                     const std::vector<Vector3f>& pts3d,
                                     const Matrix4f &proj,
                                     const IMapScoped<const float>& imap ) const;
    };

}
