/*
            CVT - Computer Vision Tools Library

     Copyright (c) 2012, Philipp Heise, Sebastian Klose

    THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
    KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
    IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
    PARTICULAR PURPOSE.
 */
#ifndef CVT_KLT_FEATURE_TRACKING_H
#define CVT_KLT_FEATURE_TRACKING_H

#include <cvt/vision/slam/stereo/FeatureTracking.h>
#include <cvt/vision/slam/stereo/DescriptorDatabase.h>
#include <cvt/vision/KLTPatch.h>
#include <cvt/math/GA2.h>


namespace cvt
{
    class KLTTracking : public FeatureTracking
    {
        public:
            KLTTracking();
            ~KLTTracking();

            /**
             * \brief track features in the current Image
             * \param trackedPositions		positions of the tracked 2D features
             * \param trackedFeatureIds		ids (of predictedIds) that have been tracked
             * \param predictedPositions	vector of predicted feature positions
             * \param img			the current image
             */
            void trackFeatures( PointSet2d&                     trackedPositions,
                                std::vector<size_t>&            trackedFeatureIds,
                                const std::vector<Vector2f>&	predictedPositions,
                                const std::vector<size_t>&      predictedIds,
                                const ImagePyramid &            pyr );

            void trackFeatures( PointSet2d&                    trackedPositions,
                                std::vector<size_t>&           trackedFeatureIds,
                                const std::vector<Vector2f>&	predictedPositions,
                                const std::vector<size_t>&		predictedIds,
                                const Image&                   img )
            {
                throw CVTException( "Not implemented" );
            }

            /**
             * \brief add a new feature to the database! (e.g. after triangulation)
             */
            void addFeatureToDatabase( const ImagePyramid &pyr,
                                       const ImagePyramid &pyrGradX,
                                       const ImagePyramid &pyrGradY,
                                       const Vector2f &f, size_t id );
            void clear();

            void addFeatureToDatabase( const Vector2f &, size_t )
            {
                throw CVTException( "NOT IMPLEMENTED" );
            }

        private:
            typedef GA2<float>          PoseType;
            static const size_t         PatchSize = 16;
            typedef KLTPatch<PatchSize, PoseType> PatchType;

            std::vector<PatchType*>     _patchForId;
            float                       _ssdThreshold;
            float                       _sadThreshold;
    };
}
#endif

