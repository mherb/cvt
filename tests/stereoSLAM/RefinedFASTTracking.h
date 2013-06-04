#ifndef CVT_REFINED_FASTTRACKING_H
#define CVT_REFINED_FASTTRACKING_H

#include <cvt/vision/features/FAST.h>
#include <cvt/vision/ImagePyramid.h>
#include <cvt/vision/FeatureFilter.h>
#include <cvt/vision/KLTPatch.h>
#include <cvt/util/Util.h>
#include <cvt/math/GA2.h>

#include <vector>
#include <set>

namespace cvt
{
	class RefinedFASTTracking
    {
        public:
            RefinedFASTTracking();
            ~RefinedFASTTracking();

            void trackFeatures( PointSet2d&                     trackedPositions,
                                std::vector<size_t>&            trackedFeatureIds,
                                const std::vector<Vector2f>&	predictedPositions,
                                const std::vector<size_t>&      predictedIds,
                                const Image&                    img );

            /**
             * \brief add a new feature to the database! (e.g. after triangulation)
             */
            void addFeatureToDatabase( const Vector2f & f, size_t id );
            void clear();

			const FeatureSet&			lastDetectedFeatures()  const { return _currentFeatures; }
			const std::set<size_t>&		associatedFeatures()    const { return _associatedIndexes; }
			const ImagePyramid&			pyramid()               const { return _pyramid; }

        private:
            static const size_t PatchSize = 16;
            typedef GA2<float>							PoseType;
            typedef KLTPatch<PatchSize, PoseType>		PatchType;

            FAST					_detector;
            ImagePyramid			_pyramid;
            ImagePyramid			_pyrf;
            ImagePyramid			_pyrGx;
            ImagePyramid			_pyrGy;
            IKernel                 _kx, _ky;
            size_t                  _kltIters;

            size_t					_fastMatchingWindowSqr;
            float					_fastMaxSADThreshold;
            float					_kltSSDThreshold;

            std::vector<PatchType*> _patchForId;

			FeatureSet				_currentFeatures;
            std::set<size_t>		_associatedIndexes;

            /* cache the simd instance here */
            SIMD*					_simd;

            void detectCurrentFeatures( const Image& img );
            int bestFASTFeatureInRadius( const PatchType& patch );
    };
}

#endif
