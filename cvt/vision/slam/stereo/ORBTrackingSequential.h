#ifndef ORB_TRACKINGSEQUENTIAL_H
#define ORB_TRACKINGSEQUENTIAL_H

#include <cvt/gfx/GFXEngineImage.h>
#include <cvt/gfx/GFX.h>

#include <cvt/vision/Vision.h>
#include <cvt/vision/ORB.h>
#include <cvt/vision/slam/MapFeature.h>
#include <cvt/vision/slam/stereo/FeatureTracking.h>
#include <cvt/vision/slam/stereo/DescriptorDatabase.h>
#include <cvt/vision/slam/stereo/ORBStereoMatching.h>

namespace cvt
{
	/**
	 *	Feature Tracking using ORB Matching	
	 */
	class ORBTrackingSequential : public FeatureTracking 
	{
		public:
			ORBTrackingSequential( const CameraCalibration& c0, const CameraCalibration & c1 );
			~ORBTrackingSequential();

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
			DescriptorDatabase<ORBFeature>	_descriptors;
			size_t							_maxDescDistance;
			float							_windowRadius;

			/* descriptor matching parameters */
			float							_matcherMaxLineDistance;	
			float							_maxTriangReprojError;
			ORBStereoMatching				_stereoMatcher;

			/* orb parameters */
			size_t							_orbOctaves;
			float							_orbScaleFactor;
			uint8_t							_orbCornerThreshold;
			size_t							_orbMaxFeatures;
			bool							_orbNonMaxSuppression;
			ORB								_orb0;
			std::set<size_t>				_orb0MatchedIds;

			// feature IDs of the features tracked in the last frame	
			std::vector<size_t>				_lastTrackedIds;
			// positions of the features in the last frame
			std::vector<ORBFeature>			_lastTrackedFeatures;
			
			std::vector<size_t>				_predictedIds;
			std::vector<Vector2f>			_predictedPositions;


			Image							_debug;

			void trackSequential( PointSet3d& p3d, PointSet2d& p2d, const SlamMap& map, const Image& image );
			void trackNonSequential( PointSet3d& p3d, PointSet2d& p2d, const SlamMap& map, const SE3<double>& pose, const Image& image );

			int matchInWindow( FeatureMatch& match, const Vector2f & p, const ORB & orb, std::set<size_t> & used ) const;
			template <class Container>
				void drawPointsInImage( Image& debug, const Color& c, const Container& points, size_t offset = 0 );

			template <class ORBContainer>
				void drawOrbsInImage( Image& debug, const Color& c, const ORBContainer& points );

			bool rangeCheck( const Vector2f& p, float range ) const;
	};

	template <class Container>
		inline void ORBTrackingSequential::drawPointsInImage( Image& debug, 
															 const Color& c, 
															 const Container& points,
															 size_t offset	)
		{
			{
				GFXEngineImage ge( debug );
				GFX g( &ge );
				g.color() = c;

				Recti r;
				size_t pSize = 5;
				size_t phSize = pSize >> 1;
				r.width = pSize;
				r.height = pSize;
				for( size_t i = offset; i < points.size(); i++ ){
					r.x = ( int )points[ i ][ 0 ] - phSize;
					r.y = ( int )points[ i ][ 1 ] - phSize;
					g.drawRect( r );
				}
			}
		}

	template <class ORBContainer>
		inline void ORBTrackingSequential::drawOrbsInImage( Image& debug, 
														   const Color& c, 
														   const ORBContainer& points )
		{
			{
				GFXEngineImage ge( debug );
				GFX g( &ge );
				g.color() = c;

				Recti r;
				size_t pSize = 5;
				size_t phSize = pSize >> 1;
				r.width = pSize;
				r.height = pSize;
				for( size_t i = 0; i < points.size(); i++ ){
					r.x = ( int )points[ i ].pt[ 0 ] - phSize;
					r.y = ( int )points[ i ].pt[ 1 ] - phSize;
					g.drawRect( r );
				}
			}
		}
}

#endif

