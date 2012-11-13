#ifndef CVT_FAST_FEATURE_TRACKING_H
#define CVT_FAST_FEATURE_TRACKING_H

#include <cvt/vision/FAST.h>
#include <cvt/vision/ImagePyramid.h>
#include <cvt/vision/FeatureFilter.h>
#include <cvt/vision/KLTPatch.h>
#include <cvt/util/Util.h>
#include <cvt/math/GA2.h>

#include <vector>
#include <set>

namespace cvt
{
	class FASTFeatureTracking
	{
		public:
			static const size_t PatchSize = 16;
			typedef GA2<float>							PoseType;
			typedef KLTPatch<PatchSize, PoseType>		PatchType;

			FASTFeatureTracking();
			~FASTFeatureTracking();

			/**
			 *	\brief try to track features in current image
			 *	\param predicted	predicted visible patches (including positions)
			 */
			void trackFeatures( std::vector<PatchType*>& tracked,
								std::vector<PatchType*>& lost,
								std::vector<PatchType*>& predicted,
								const Image& image );

			void setScaleFactor( float v )		  { _pyramid.setScaleFactor( v ); }
			void setFASTThreshold( uint8_t v )    { _detector.setThreshold( v ); }
			void setFASTSADThreshold( float v )   { _fastMaxSADThreshold = v * 255; }
			void setKLTSSDThreshold( float v )	  { _kltSSDThreshold = Math::sqr( v ); }
			void setNonMaxSuppression( bool v )	  { _detector.setNonMaxSuppress( v ); }
			void setMaxMatchingRadius( size_t r ) { _fastMatchingWindowSqr = Math::sqr( r ); DEBUG_PRINT(r)}

            const std::vector<Feature2Df>&  lastDetectedFeatures()  const { return _currentFeatures; }
            const std::set<size_t>&         associatedFeatures()    const { return _associatedIndexes; }
            const ImagePyramid&             pyramid()               const { return _pyramid; }
            const ImagePyramid&             pyramidFloat()          const { return _pyrf; }

            void addFeatures( std::vector<PatchType*>& featureContainer, const std::vector<Vector2f>& newFeatures );

		private:
			FAST					_detector;
			ImagePyramid			_pyramid;
			ImagePyramid			_pyrf;
			ImagePyramid			_pyrGx;
			ImagePyramid			_pyrGy;
			IKernel					_kx, _ky;


            size_t					_fastMatchingWindowSqr;
            float					_fastMaxSADThreshold;
            float					_kltSSDThreshold;

			std::vector<Feature2Df>	_currentFeatures;
			std::set<size_t>		_associatedIndexes;

			/* cache the simd instance here */
			SIMD*					_simd;

			void detectCurrentFeatures( const Image& img );

			int bestFASTFeatureInRadius( const PatchType& patch );

	};
}

#endif
