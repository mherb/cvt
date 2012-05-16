/*
            CVT - Computer Vision Tools Library

     Copyright (c) 2012, Philipp Heise, Sebastian Klose

    THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
    KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
    IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
    PARTICULAR PURPOSE.
 */

#ifndef CVT_PATCHSTEREOINIT_H
#define CVT_PATCHSTEREOINIT_H

#include <cvt/vision/slam/stereo/DepthInitializer.h>
#include <cvt/vision/FAST.h>
#include <cvt/vision/ImagePyramid.h>

namespace cvt {

    class PatchStereoInit : public DepthInitializer
    {
        public:
            PatchStereoInit( const CameraCalibration& c0, const CameraCalibration& c1 );
            ~PatchStereoInit();

            void        triangulateFeatures( std::vector<DepthInitResult> & triangulated,
                                             const std::vector<Vector2f>  & avoidPositionsImg0,
                                             const Image& view0, const Image& view1 );

            ParamSet&   parameters()    { return _pset; }

            struct Parameters
            {
                uint32_t maxNewFeatures;
                uint64_t maxSAD;
                float    maxEpilineDistance;
                float    maxReprojectionError;
                float    maxDepth;
                float    minDepth;
                uint8_t  fastThreshold;
                uint16_t gridSize;
                float    minInterFeatureDistance;
            };

        private:
            static const size_t PatchSize = 32;

            FAST            _detector;
            ImagePyramid    _pyramidView0;
            ImagePyramid    _pyramidView1;
            ParamSet        _pset;
            Parameters*     _params;

            Matrix3f        _fundamental;

            SIMD*           _simd;

            void detectFeatures( std::vector<Feature2Df>& features, const ImagePyramid& pyramid );

            void filterFeatures( std::vector<Vector2f>& filtered,
                                 const std::vector<Feature2Df>& features,
                                 const std::vector<Vector2f>& avoid );

            void matchAndTriangulate( std::vector<DepthInitResult>& triangulated,
                                      const std::vector<Vector2f>& f0,
                                      const std::vector<Feature2Df>& f1 );

            size_t computePatchSAD( const uint8_t* p0, size_t s0,
                                    const uint8_t* p1, size_t s1 ) const;


    };

}

#endif
