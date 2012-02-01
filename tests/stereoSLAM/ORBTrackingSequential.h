#ifndef ORB_TRACKINGSEQUENTIAL_H
#define ORB_TRACKINGSEQUENTIAL_H

#include "FeatureTracking.h"
#include <cvt/vision/slam/MapFeature.h>
#include "DescriptorDatabase.h"
#include <cvt/vision/ORB.h>
#include <cvt/vision/Vision.h>

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


			Image							_debug;

			void trackSequential( PointSet3d& p3d, PointSet2d& p2d, const SlamMap& map, const Image& image );
			void trackNonSequential( PointSet3d& p3d, PointSet2d& p2d, const SlamMap& map, const SE3<double>& pose, const Image& image );

			int matchInWindow( FeatureMatch& match, const Vector2f & p, const ORB & orb, std::set<size_t> & used ) const;
			template <class Container>
			void drawPointsInImage( Image& debug, const Color& c, const Container& points, size_t offset = 0 );

			template <class ORBContainer>
			void drawOrbsInImage( Image& debug, const Color& c, const ORBContainer& points );
	};

	inline ORBTrackingSequential::ORBTrackingSequential( const CameraCalibration & c0, const CameraCalibration & c1 ) :
		_camCalib0( c0 ),
		_camCalib1( c1 ),
		_maxDescDistance( 70 ),
		_windowRadius( 80 ),
		_matcherMaxLineDistance( 5.0f ),
		_maxTriangReprojError( 5.0f ),
		_stereoMatcher( _matcherMaxLineDistance, _maxDescDistance, c0, c1 ),
		_orbOctaves( 3 ), 
		_orbScaleFactor( 0.5f ),
		_orbCornerThreshold( 25 ),
		_orbMaxFeatures( 1200 ),
		_orbNonMaxSuppression( true ),
		_orb0( _orbOctaves, _orbScaleFactor, _orbCornerThreshold, _orbMaxFeatures, _orbNonMaxSuppression )
	{
	}

	inline ORBTrackingSequential::~ORBTrackingSequential()
	{
	}

	inline void ORBTrackingSequential::trackFeatures( PointSet3d&			p3d, 
													  PointSet2d&			p2d,
													  const SlamMap&		map,
													  const SE3<double>&	pose,
													  const Image&			img )
	{
		// create the ORB
		_orb0.update( img );
		img.convert( _debug, IFormat::RGBA_UINT8 );
		
		drawOrbsInImage( _debug, Color::BLUE, _orb0 );
		drawOrbsInImage( _debug, Color::RED, _lastTrackedFeatures );
		trackSequential( p3d, p2d, map, img );
		drawPointsInImage( _debug, Color::GREEN, p2d );

		size_t offset = p2d.size();
		trackNonSequential( p3d, p2d, map, pose, img );
		drawPointsInImage( _debug, Color::GREEN, p2d, offset );
		debugImage.notify( _debug );
	}

	inline void ORBTrackingSequential::trackSequential( PointSet3d&			p3d, 
													    PointSet2d&			p2d,
												        const SlamMap&		map,
													    const Image&		img )
    {
		// keep track of already assigned indices to avoid double associations
		Vector3d p3;
		Vector2d p2;

		_orb0MatchedIds.clear();
		size_t savePos = 0;
		for( size_t i = 0; i < _lastTrackedFeatures.size(); i++ ){
			FeatureMatch m;
			m.feature0 = &_lastTrackedFeatures[ i ];
			int orbid = matchInWindow( m, _lastTrackedFeatures[ i ].pt, _orb0, _orb0MatchedIds );
			if( m.feature1 ){
				// got a match so add it to the point sets
				size_t currId = _lastTrackedIds[ i ];
				const MapFeature & mapFeat = map.featureForId( currId );
				size_t keyframeId = *( mapFeat.pointTrackBegin() );
				if( checkFeatureSAD( m.feature0->pt, m.feature1->pt, 
									 map.keyframeForId( keyframeId ).image(), img ) ) {
					p3.x = mapFeat.estimate().x(); 
					p3.y = mapFeat.estimate().y(); 
					p3.z = mapFeat.estimate().z();
					p3d.add( p3 );
					p2.x = m.feature1->pt.x;
					p2.y = m.feature1->pt.y;
					p2d.add( p2 );
					_orb0MatchedIds.insert( orbid );
					_lastTrackedIds[ savePos ] = currId;
					_lastTrackedFeatures[ savePos ] = *( ORBFeature* )m.feature1;
					savePos++;					
				}
			}
		}

		_lastTrackedFeatures.erase( _lastTrackedFeatures.begin() + savePos, _lastTrackedFeatures.end() );
		_lastTrackedIds.erase( _lastTrackedIds.begin() + savePos, _lastTrackedIds.end() );
    }


	inline void ORBTrackingSequential::trackNonSequential( PointSet3d&			p3d, 
														   PointSet2d&			p2d,
														   const SlamMap&		map,
														   const SE3<double>&	pose,
														   const Image&			img )
	{
		// predict visible features with map and current pose
		std::vector<size_t>		predictedIds;
		std::vector<Vector2f>	predictedPositions;
		map.selectVisibleFeatures( predictedIds, predictedPositions, pose.transformation(), _camCalib0 );

		drawPointsInImage( _debug, Color::YELLOW, predictedPositions );

		// remove the already tracked ids:
		bool hit;
			
		size_t savePos = 0;
		for( size_t k = 0; k < predictedIds.size(); k++ ){
			hit = false;
			for( size_t i = 0; i < _lastTrackedIds.size(); i++ ){
				if( _lastTrackedIds[ i ] == predictedIds[ k ] ){
					hit = true;
					break;
				}
			}
			if( !hit ){
				if( savePos != k ){
					predictedIds[ savePos ] = predictedIds[ k ];
					predictedPositions[ savePos ] = predictedPositions[ k ];
					savePos++;
				}
			}
		}
		predictedIds.erase( predictedIds.begin() + savePos, predictedIds.end() );
		predictedPositions.erase( predictedPositions.begin() + savePos, predictedPositions.end() );

        // we want to find the best matching orb feature from current, that lies
        // within a certain distance from the "predicted" position
        std::vector<size_t>::const_iterator currentId = predictedIds.begin();
        std::vector<size_t>::const_iterator tEnd = predictedIds.end();
        std::vector<Vector2f>::const_iterator pred = predictedPositions.begin();

		// keep track of already assigned indices to avoid double associations
		Vector3d p3;
		Vector2d p2;
        while( currentId != tEnd ){
			FeatureMatch m;
			const ORBFeature & desc = _descriptors.descriptor( *currentId );
			m.feature0 = &desc;
            int orbid = matchInWindow( m, *pred, _orb0, _orb0MatchedIds );
			if( m.feature1 ){
				// got a match so add it to the point sets
				const MapFeature & mapFeat = map.featureForId( *currentId );
				size_t keyframeId = *( mapFeat.pointTrackBegin() );
				if( checkFeatureSAD( m.feature0->pt, m.feature1->pt, 
									 map.keyframeForId( keyframeId ).image(), img ) ) {
					p3.x = mapFeat.estimate().x(); 
					p3.y = mapFeat.estimate().y(); 
					p3.z = mapFeat.estimate().z();
					p3d.add( p3 );
					p2.x = m.feature1->pt.x;
					p2.y = m.feature1->pt.y;
					p2d.add( p2 );
					_orb0MatchedIds.insert( orbid );
					_lastTrackedIds.push_back( *currentId );
					_lastTrackedFeatures.push_back( *( ORBFeature* )m.feature1 );
				}
			}
			++currentId;
			++pred;
        }
	}

    inline int ORBTrackingSequential::matchInWindow( FeatureMatch& match, const Vector2f & p, const ORB & orb, std::set<size_t> & used ) const
    {
		const ORBFeature * f = (ORBFeature*)match.feature0;
        match.distance = _maxDescDistance;
        size_t currDist;
		const std::set<size_t>::const_iterator usedEnd = used.end();
		size_t matchedId = 0;
        for( size_t i = 0; i < orb.size(); i++ ){
			if( used.find( i ) == usedEnd ){
				if( ( p - orb[ i ].pt ).length() < _windowRadius ){
					// try to match
					currDist = f->distance( orb[ i ] );
					if( currDist < match.distance ){
						match.feature1 = &orb[ i ];
						match.distance = currDist;
						matchedId = i;
					}
				}
			}
        }

		// to ensure unique matches
		if( match.distance < _maxDescDistance ){
			used.insert( matchedId );
			return ( int )matchedId;
		}

		return -1;
    }

	inline size_t ORBTrackingSequential::triangulateNewFeatures( SlamMap& map,
													   const SE3<double> & pose,
													   const Image & firstView,
												       const Image & secondView )
	{
		// create the ORB
		ORB orb1( secondView, 
				 _orbOctaves, 
				 _orbScaleFactor,
				 _orbCornerThreshold,
				 _orbMaxFeatures,
				 _orbNonMaxSuppression );

		// find stereoMatches by avoiding already found matches
		std::vector<FeatureMatch> stereoMatches;

		// alreadyMatchedIndices are the indices of orb0 that have been matched before
		_stereoMatcher.matchEpipolar( stereoMatches, _orb0, orb1, _orb0MatchedIds );

		size_t numNew = 0;
		// Create a new keyframe with image 0 as reference image
		if( stereoMatches.size() > 0 ){

			// add a new keyframe to the map
			size_t kId = map.addKeyframe( pose.transformation() );

			Keyframe & keyframe = map.keyframeForId( kId );
			keyframe.setImage( firstView );

			MapMeasurement meas;
			// meas.information *= ( 1.0 / _trackingSearchRadius );

			Eigen::Matrix4d featureCov = Eigen::Matrix4d::Identity();
			MapFeature mapFeat( Eigen::Vector4d::Zero(), featureCov );

			// add the currently tracked features to the keyframe!
			for( size_t i = 0; i < _lastTrackedIds.size(); i++ ){				
				size_t pointId = _lastTrackedIds[ i ];
				meas.point[ 0 ] = _lastTrackedFeatures[ i ].pt.x;
				meas.point[ 1 ] = _lastTrackedFeatures[ i ].pt.y;
				map.addMeasurement( pointId, kId, meas );
			}

			Vector2f p0, p1;
			Vector4f pNew;

			Eigen::Matrix4d poseInv = pose.transformation().inverse();
			for( size_t i = 0; i < stereoMatches.size(); i++ ){
				const FeatureMatch& match = stereoMatches[ i ];
				if( match.feature1 ){
					if( checkFeatureSAD( match.feature0->pt, match.feature1->pt, firstView, secondView ) ){
						p0 = stereoMatches[ i ].feature0->pt;
						p1 = stereoMatches[ i ].feature1->pt;
						Vision::correctCorrespondencesSampson( p0, p1, _stereoMatcher.fundamental() );

						float reprErr = triangulateSinglePoint( pNew, p0, p1, _camCalib0.projectionMatrix(), _camCalib1.projectionMatrix() );
						if( reprErr < _maxTriangReprojError ){
							meas.point[ 0 ] = p0.x;
							meas.point[ 1 ] = p0.y;

							Eigen::Vector4d & point = mapFeat.estimate();
							point[ 0 ] = pNew[ 0 ];
							point[ 1 ] = pNew[ 1 ];
							point[ 2 ] = pNew[ 2 ];
							point[ 3 ] = pNew[ 3 ];
							point = poseInv * point;

							size_t newPointId = map.addFeatureToKeyframe( mapFeat, meas, kId );
							_descriptors.addDescriptor( *( ORBFeature* )stereoMatches[ i ].feature0, newPointId );
							_lastTrackedIds.push_back( newPointId );
							_lastTrackedFeatures.push_back( *( ORBFeature* )stereoMatches[ i ].feature0 );
							numNew++;
						}
					}
				}
			}
		}
		std::cout << "New Features Triangulated: " << numNew << std::endl;
		return numNew;
	}

	inline void ORBTrackingSequential::clear()
	{
		_descriptors.clear();
		_lastTrackedIds.clear();
		_lastTrackedFeatures.clear();
	}


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
