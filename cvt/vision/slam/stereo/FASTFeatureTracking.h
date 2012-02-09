#ifndef FAST_FEATURE_TRACKING_H
#define FAST_FEATURE_TRACKING_H

#include <cvt/gfx/GFXEngineImage.h>
#include <cvt/gfx/GFX.h>

#include <cvt/vision/Vision.h>
#include <cvt/vision/FAST.h>
#include <cvt/vision/ImagePyramid.h>
#include <cvt/vision/FeatureRowLookupContainer.h>
#include <cvt/vision/slam/MapFeature.h>
#include <cvt/vision/slam/stereo/FeatureTracking.h>

namespace cvt
{
	/**
	 *	Feature Tracking using FAST, adopted from PTAM paper
	 *		find best matching FAST feature in a certain window around
	 *		previous and predicted positions
	 */
	class FASTFeatureTracking : public FeatureTracking 
	{
		public:
			FASTFeatureTracking( const CameraCalibration& c0, const CameraCalibration & c1 );
			~FASTFeatureTracking();

			void trackFeatures( PointSet3d&			p3d, 
							    PointSet2d&			p2d,
							    const SlamMap&		map,
							    const SE3<double>&	pose,
							    const Image&		img );

			size_t triangulateNewFeatures( SlamMap& map, 
										   const SE3<double>& pose, 
										   const Image& first, 
										   const Image& second );

			void clear();

		private:
			const CameraCalibration&		_camCalib0;
			const CameraCalibration&		_camCalib1;
		
			// scale space
			ImagePyramid*					_currentPyramid;
			ImagePyramid*					_lastPyramid;

			// fast detector stuff
			uint8_t									_fastThreshold;
			size_t									_fastBorder;
			std::vector<FeatureRowLookupContainer>	_fastContainer;
			std::vector<std::set<size_t> >			_usedIdsForOctave;
			
			// search radius for fast features
			float							_windowRadius;

			/* descriptor matching parameters */
			float							_matcherMaxLineDistance;	
			float							_maxTriangReprojError;

			// feature IDs of the features tracked in the last frame	
			std::vector<size_t>				_lastTrackedIds;
			// positions of the features in the last frame
			std::vector<Feature2Df>			_lastTrackedFeatures;
			
			std::vector<size_t>				_predictedIds;
			std::vector<Vector2f>			_predictedPositions;


			Image							_debug;

			void trackSequential( PointSet3d& p3d, PointSet2d& p2d, const SlamMap& map, const Image& image );
			void trackNonSequential( PointSet3d& p3d, PointSet2d& p2d, const SlamMap& map, const SE3<double>& pose, const Image& image );

			
			//int matchInWindow( FeatureMatch& match, const Vector2f & p, const ORB & orb, std::set<size_t> & used ) const;

			// detect fastFeatures in all scales
			void detectFeatures();

			void swapPyramids();

	};

}

#endif

