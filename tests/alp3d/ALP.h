#include <Eigen/Core>
#include <WarpSet.h>
#include <cvt/math/Vector.h>

namespace cvt {

    /* this is the class holding the current overall predictor*/
    class ALP3D {
        public:
            ALP3D();

            void train( const std::vector<Vector3f>& samplePoints, const Image& gray );

            /**
             * @brief track track the pose of the camera using the current predictors
             * @param pose
             * @param gray
             */
            void track( Matrix4f& pose, const Image& gray );

        private:
            // predictor matrix
            typedef Eigen::Matrix<float, 6, Eigen::Dynamic>              LPMat;
            // matrix of pose vector
            typedef Eigen::Matrix<float, Eigen::Dynamic, 6>              YMat;
            typedef Eigen::Matrix<float, Eigen::Dynamic, Eigen::Dynamic> HMat;

            LPMat       _predictor;
            WarpSet     _warpSet;

            std::vector<Vector3f>   _currentPoints3d;

            void updatePoseVector()
            {
                // generate n new samples in the warpSet
                // compute the necessary update to the predictor
            }

    };

}
