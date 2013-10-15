#ifndef CVT_FEATUREMATCHER_H
#define CVT_FEATUREMATCHER_H

#include <cvt/vision/FeatureMatch.h>
#include <cvt/vision/ORB2.h>
#include <cvt/vision/ORB.h>
#include <set>

namespace cvt
{
	class FeatureMatcher
	{
		public:
			template <class FeatureContainerType0, class FeatureContainerType1>
			static void matchBruteForce( std::vector<FeatureMatch> & matches, 
										 const FeatureContainerType0 & f0, 
										 const FeatureContainerType1 & f1,
										 float maxDistance );
			
			template <class FeatureContainerType0, class FeatureContainerType1>
			static void matchWithWindow( std::vector<FeatureMatch> & matches, 
									     const FeatureContainerType0 & f0, 
									     const FeatureContainerType1 & f1,
										 float maxDistance, float maxDescDistance );
			
	};

	template<>
	inline void FeatureMatcher::matchBruteForce<ORB2, ORB2>( std::vector<FeatureMatch> & matches, 
													   const ORB2 & orb0, 
													   const ORB2 & orb1, 
													   float maxDistance )
	{
		FeatureMatch m;

		// brighter features
		for( size_t i = 0; i < orb0.numBrighter(); i++ ){
			const ORB2Feature & f = orb0.brighterFeature( i );
			m.feature0 = &orb0.brighterFeature( i );
			m.distance = maxDistance;
			for( size_t k = 0; k < orb1.numBrighter(); k++ ){
				size_t dist = f.distance( orb1.brighterFeature( k ) );

				if( dist < m.distance ){
					m.distance = dist;
					m.feature1 = &orb1.brighterFeature( k );
				}
			}

			if( m.distance < maxDistance ){
				matches.push_back( m );
			}
		}

		// darker features
		for( size_t i = 0; i < orb0.numDarker(); i++ ){
			const ORB2Feature & f = orb0.darkerFeature( i );
			m.feature0 = &orb0.darkerFeature( i );
			m.distance = maxDistance;
			for( size_t k = 0; k < orb1.numDarker(); k++ ){
				size_t dist = f.distance( orb1.darkerFeature( k ) );

				if( dist < m.distance ){
					m.distance = dist;
					m.feature1 = &orb1.darkerFeature( k );
				}
			}

			if( m.distance < maxDistance ){
				matches.push_back( m );
			}
		}
	}


	template<>
	inline void FeatureMatcher::matchBruteForce<ORB, ORB>( std::vector<FeatureMatch> & matches, 
													  const ORB & orb0, 
													  const ORB & orb1, 
													  float maxDistance )
	{
		FeatureMatch m;
		std::set<size_t> assigned;
		for( size_t i = 0; i < orb0.size(); i++ ){
			m.feature0 = &orb0[ i ];
			m.distance = maxDistance;
			for( size_t k = 0; k < orb1.size(); k++ ){
				if( assigned.find( k ) != assigned.end() )
					continue;

				size_t dist = orb0[ i ].distance( orb1[ k ] );

				if( dist < m.distance ){
					m.distance = dist;
					m.feature1 = &orb1[ k ];
					assigned.insert( k );
				}
			}

			if( m.distance < maxDistance ){
				matches.push_back( m );
			}
		}
	}


	template<>
	inline void FeatureMatcher::matchBruteForce<ORB, std::vector<ORBFeature> >( std::vector<FeatureMatch> & matches, 
																			    const ORB & orb0, 
																				const std::vector<ORBFeature> & orb1, 
																				float maxDistance )
	{
		FeatureMatch m;
		std::set<size_t> assigned;
		for( size_t i = 0; i < orb0.size(); i++ ){
			m.feature0 = &orb0[ i ];
			m.distance = maxDistance;
			for( size_t k = 0; k < orb1.size(); k++ ){
				if( assigned.find( k ) != assigned.end() )
					continue;

				size_t dist = orb0[ i ].distance( orb1[ k ] );

				if( dist < m.distance ){
					m.distance = dist;
					m.feature1 = &orb1[ k ];
					assigned.insert( k );
				}
			}

			if( m.distance < maxDistance ){
				matches.push_back( m );
			}
		}
	}

	template<>
	inline void FeatureMatcher::matchWithWindow<ORB, std::vector<ORBFeature> >( std::vector<FeatureMatch> & matches, 
																			    const ORB & orb0, 
																				const std::vector<ORBFeature> & orb1, 
																				float maxDistance, float maxDescDistance )
	{
		float distThreshSqr = Math::sqr( maxDistance );
		FeatureMatch m;
		std::set<size_t> assigned;
		for( size_t i = 0; i < orb0.size(); i++ ){
			m.feature0 = &orb0[ i ];
			m.distance = maxDescDistance;
			for( size_t k = 0; k < orb1.size(); k++ ){
				if( assigned.find( k ) != assigned.end() ||
					( orb0[ i ].pt - orb1[ k ].pt ).lengthSqr() > distThreshSqr )
					continue;

				size_t dist = orb0[ i ].distance( orb1[ k ] );

				if( dist < m.distance ){
					m.distance = dist;
					m.feature1 = &orb1[ k ];
					assigned.insert( k );
				}
			}

			if( m.distance < maxDescDistance ){
				matches.push_back( m );
			}
		}
	}
}

#endif
