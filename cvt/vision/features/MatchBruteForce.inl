#ifndef CVT_FEATURE_MATCHER_INL
#define CVT_FEATURE_MATCHER_INL

namespace cvt {

	namespace FeatureMatcher {
		template<typename T, typename DFUNC>
		static inline void matchBruteForce( std::vector<FeatureMatch>& matches, const std::vector<T>& seta, const std::vector<T>& setb, DFUNC dfunc, float distThreshold )
		{
			matches.reserve( seta.size() );
			for( size_t i = 0; i < seta.size(); i++ ) {
				FeatureMatch m;
				const T& d0 = seta[ i ];

				m.feature0 = &d0;
				m.feature1 = 0;
				m.distance = distThreshold;
				for( size_t k = 0; k < setb.size(); k++ ) {
					const T& d1 = setb[ k ];
					float distance = dfunc( d0, d1 );

					if( distance < m.distance ) {
						m.feature1 = &d1;
						m.distance = distance;
					}
				}

				if( m.feature1 )
					matches.push_back( m );
			}
		}
	}
}

#endif
