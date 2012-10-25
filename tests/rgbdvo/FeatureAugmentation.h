#ifndef FEATUREAUGMENTATION_H
#define FEATUREAUGMENTATION_H

#include <cvt/vision/slam/stereo/KLTTracking.h>
#include <cvt/vision/FAST.h>
#include <cvt/vision/ImagePyramid.h>

#include <map>

namespace cvt {

    class FeatureAugmentation
    {
        public:
            FeatureAugmentation( const Matrix3f& intrinsics );
            ~FeatureAugmentation();

            /**
              * \brief  track features frame by frame and triangulate new features
              */
            void trackAndTriangulate( std::vector<Vector3f>& newPoints,
                                      const Image& img,
                                      const Image& depth,
                                      const Matrix4f& pose3d );


        private:
            struct FeatureTrack {
                    std::vector<Vector2f> positions;
                    std::vector<Matrix4f> projectionMat;

            };

            Matrix3f                        _intrinsics;

            // next free feature id
            size_t                          _nextId;

            // detecting new features with fast
            FAST                            _fastDetector;

            // feature tracking using KLT
            KLTTracking                     _featureTracking;

            // current feature Tracks
            typedef std::map<size_t, FeatureTrack> MapType;
            MapType                         _featureTracks;

            void trackFeatures( const Image &img, const Matrix4f &pose );

    };

}

#endif // FEATUREAUGMENTATION_H
