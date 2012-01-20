#ifndef ORB_TRACKING_H
#define ORB_TRACKING_H

#include <cvt/vision/slam/MapFeature.h>
#include "DescriptorDatabase.h"

namespace cvt
{

	/**
	 *	Feature Tracking using ORB Matching	
	 */
	class ORBTracking 
	{
		public:
			ORBTracking( const DescriptorDatabase<ORBFeature> & database, size_t maxDescDistance, float windowRadius );
			~ORBTracking();

            void setMaxDescDist( size_t v ) { _maxDescDistance = v; }
            void setMatchRadius( size_t v ) { _windowRadius = v; }

            void trackFeatures( std::vector<FeatureMatch> & matches, 
							    const std::vector<size_t> & predictedFeatureIds,
								const std::vector<Vector2f> & predictedPositions,
							    const ORB& orb ) const;

			bool checkFeature( const FeatureMatch & match, const Image & i0, const Image & i1 ) const;

		private:
			const DescriptorDatabase<ORBFeature>&	_descriptors;
            size_t									_maxDescDistance;
            float									_windowRadius;
			float									_maxSAD;
            
			void matchInWindow( FeatureMatch& match, const Vector2f & p, const ORB & orb, std::set<size_t> & used ) const;
	};

	inline ORBTracking::ORBTracking( const DescriptorDatabase<ORBFeature> & ddb, 
											 size_t maxDD, 
											 float wR ) :
		_descriptors( ddb ),
		_maxDescDistance( maxDD ),
		_windowRadius( wR )
	{
	}

	inline ORBTracking::~ORBTracking()
	{
	}

    inline void ORBTracking::trackFeatures( std::vector<FeatureMatch>&	 matches, 
												const std::vector<size_t>&	 predictedFeatureIds,
											    const std::vector<Vector2f>& predictedPositions,
                                                const ORB&					 orb ) const
    {
        // we want to find the best matching orb feature from current, that lies
        // within a certain distance from the "predicted" position
        std::vector<size_t>::const_iterator currentId = predictedFeatureIds.begin();
        std::vector<size_t>::const_iterator tEnd = predictedFeatureIds.end();
        std::vector<Vector2f>::const_iterator  pred = predictedPositions.begin();

		float avgDist = 0.0f;
		size_t numMatched = 0;

		// keep track of already assigned indices to avoid double associations
		std::set<size_t> used;
        while( currentId != tEnd ){
			FeatureMatch m;
			const ORBFeature & desc = _descriptors.descriptor( *currentId );
			m.feature0 = &desc;
            matchInWindow( m, *pred, orb, used );
			if( m.feature1 ){
				numMatched++;
				avgDist += ( *pred - m.feature1->pt ).length();
			}
			matches.push_back( m );
			++currentId;
			++pred;
        }
    }

    inline void ORBTracking::matchInWindow( FeatureMatch& match, const Vector2f & p, const ORB & orb, std::set<size_t> & used ) const
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
		}
    }


	inline bool ORBTracking::checkFeature( const FeatureMatch & match, const Image & i0, const Image & i1 ) const
	{
		size_t s0, s1;
		const uint8_t* ptr0 = i0.map( &s0 );
		const uint8_t* ptr1 = i1.map( &s1 );

		const uint8_t* p0 = ptr0 + ( (int)match.feature0->pt.y - 8 ) * s0 + ( (int)match.feature0->pt.x - 8 );
		const uint8_t* p1 = ptr1 + ( (int)match.feature1->pt.y - 8 ) * s1 + ( (int)match.feature1->pt.x - 8 );

		float sad = 0;
		for( size_t i = 0; i < 16; i++ ){	
			sad += SIMD::instance()->SAD( p0, p1, 16 );
			p0 += s0;
			p1 += s1;
		}

		i0.unmap( ptr0 );
		i1.unmap( ptr1 );

		// average & normalize
		sad = 1.0f - ( sad / Math::sqr( 256.0 ) );
		
		if( sad > 0.7f )
			return true;
		return false;
	}
}

#endif
