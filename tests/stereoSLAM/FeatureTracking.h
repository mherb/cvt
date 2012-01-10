#ifndef FEATURE_TRACKING_H
#define FEATURE_TRACKING_H

#include <cvt/vision/slam/MapFeature.h>
#include "DescriptorDatabase.h"

namespace cvt
{

	/**
	 *	Feature Tracking using ORB Matching	
	 */
	class FeatureTracking 
	{
		public:
			FeatureTracking( const DescriptorDatabase<ORBFeature> & database, size_t maxDescDistance, float windowRadius );
			~FeatureTracking();

            void setMaxDescDist( size_t v ) { _maxDescDistance = v; }
            void setMatchRadius( size_t v ) { _windowRadius = v; }

            void trackFeatures( std::vector<FeatureMatch> & matches, 
							    const std::vector<size_t> & predictedFeatureIds,
								const std::vector<Vector2f> & predictedPositions,
							    const ORB& orb ) const;

		private:
			const DescriptorDatabase<ORBFeature>&	_descriptors;
            size_t									_maxDescDistance;
            float									_windowRadius;
            
			void matchInWindow( FeatureMatch& match, const Vector2f & p, const ORB & orb ) const;
	};

	inline FeatureTracking::FeatureTracking( const DescriptorDatabase<ORBFeature> & ddb, 
											 size_t maxDD, 
											 float wR ) :
		_descriptors( ddb ),
		_maxDescDistance( maxDD ),
		_windowRadius( wR )
	{
	}

	inline FeatureTracking::~FeatureTracking()
	{
	}

    inline void FeatureTracking::trackFeatures( std::vector<FeatureMatch>&	 matches, 
												const std::vector<size_t>&	 predictedFeatureIds,
											    const std::vector<Vector2f>& predictedPositions,
                                                const ORB&					 orb ) const
    {
        // we want to find the best matching orb feature from current, that lies
        // within a certain distance from the "predicted" position
        std::vector<size_t>::const_iterator currentId = predictedFeatureIds.begin();
        std::vector<size_t>::const_iterator tEnd = predictedFeatureIds.end();
        std::vector<Vector2f>::const_iterator  pred = predictedPositions.begin();

        while( currentId != tEnd ){
			FeatureMatch m;
			const ORBFeature & desc = _descriptors.descriptor( *currentId );
			m.feature0 = &desc;
            matchInWindow( m, *pred, orb );
			matches.push_back( m );
			++currentId;
			++pred;
        }
    }

    inline void FeatureTracking::matchInWindow( FeatureMatch& match, const Vector2f & p, const ORB & orb ) const
    {
		const ORBFeature * f = (ORBFeature*)match.feature0;
        match.distance = _maxDescDistance;
        size_t currDist;
        for( size_t i = 0; i < orb.size(); i++ ){
            if( ( p - orb[ i ].pt ).length() < _windowRadius ){
                // try to match
                currDist = f->distance( orb[ i ] );
                if( currDist < match.distance ){
					match.feature1 = &orb[ i ];
                    match.distance = currDist;
                }
            }
        }
    }
}

#endif
