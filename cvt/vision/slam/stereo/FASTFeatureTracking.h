#ifndef FAST_FEATURE_TRACKING_H
#define FAST_FEATURE_TRACKING_H

#include <cvt/gfx/GFXEngineImage.h>
#include <cvt/gfx/GFX.h>
#include <cvt/geom/Line2D.h>
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

			// scale factors to scale up the features!
			std::vector<float>				_scaleFactors;

			// fast detector stuff
			uint8_t									_fastThreshold;
			size_t									_fastBorder;
			std::vector<FeatureRowLookupContainer>	_fastContainer;
			std::vector<std::set<size_t> >			_usedIdsForOctave;
			
			// search radius for fast features
			float							_windowRadius;
			size_t							_patchSize;
			float							_sadThreshold;

			/* descriptor matching parameters */
			float							_maxLineDist;	
			float							_maxTriangReprojError;
			Matrix3f						_fundamental;

			// feature IDs of the features tracked in the last frame	
			std::vector<size_t>				_lastTrackedIds;
			// positions of the features in the last frame
			std::vector<Feature2Df>			_lastTrackedFeatures;
			
			std::vector<size_t>				_predictedIds;
			std::vector<Vector2f>			_predictedPositions;


			Image							_debug;


			void trackSequential( PointSet3d& p3d, PointSet2d& p2d, const SlamMap& map, const Image& image );
			void trackNonSequential( PointSet3d& p3d, PointSet2d& p2d, const SlamMap& map, const SE3<double>& pose, const Image& image );

			// detect fastFeatures in all scales
			void detectFeatures();

			void swapPyramids();

			/* using SAD, find the closest match */
			float findBestMatchInWindow( Feature2Df& match, 
										 const uint8_t* last, size_t lstride, 
										 const Feature2Df& lastF ) const;

			float findBestMatchOnLine( Feature2Df& match,
									   const Line2Df& epiline,
									   size_t octave,
									   const uint8_t* ptrA, size_t strideA,
									   const uint8_t* patch, size_t patchStride ) const;

			bool alreadyTrackingFeatureInRange( const Feature2Df& f, float range ) const;

			size_t patchSAD( const uint8_t* ptrA, size_t strideA, 
							 const uint8_t* ptrB, size_t strideB ) const;

			float triangulate( Vector2f& p0, Vector2f& p1, Vector4f& ptriang ) const;

			template <class Container>	
			void drawPointsInImage( Image& debug, const Color& c, const Container& points, size_t offset = 0 ) const;

	};
			
	template <class Container>	
	void FASTFeatureTracking::drawPointsInImage( Image& debug, const Color& c, const Container& points, size_t offset ) const
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

	template <>	
	void FASTFeatureTracking::drawPointsInImage<std::vector<Feature2Df> >( Image& debug, const Color& c, const std::vector<Feature2Df>& points, size_t offset ) const
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
				r.x = ( int )points[ i ].pt.x * _scaleFactors[ points[ i ].octave ] - phSize;
				r.y = ( int )points[ i ].pt.y * _scaleFactors[ points[ i ].octave ] - phSize;
				g.drawRect( r );
			}
		}
	}


	template <>	
	void FASTFeatureTracking::drawPointsInImage<FeatureRowLookupContainer>( Image& debug, const Color& c, const FeatureRowLookupContainer& points, size_t ) const
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

			for( size_t i = 0; i < points.numRows(); i++ ){
				FeatureRowLookupContainer::ConstRowIterator rowIter = points.rowBegin( i );
				const FeatureRowLookupContainer::ConstRowIterator rowEnd = points.rowEnd( i );
				while( rowIter != rowEnd ){
					r.x = ( int )rowIter->pt.x * _scaleFactors[ rowIter->octave ] - phSize;
					r.y = ( int )rowIter->pt.y * _scaleFactors[ rowIter->octave ] - phSize;
					g.drawRect( r );
					rowIter++;
				}
			}
		}
	}

}

#endif

