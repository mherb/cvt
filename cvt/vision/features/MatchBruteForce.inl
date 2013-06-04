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

		template<typename T, typename DFUNC>
		static inline void matchInWindow( std::vector<MatchingIndices>& matches,
										  const std::vector<FeatureDescriptor*>& setA,
										  const std::vector<T>& setB,
										  DFUNC dfunc,
										  float maxFeatureDist,
										  float maxDescDistance )
		{
			matches.reserve( setA.size() );
			MatchingIndices m;
			float distanceSquare = Math::sqr( maxFeatureDist );
			for( size_t i = 0; i < setA.size(); ++i ) {
				const T& d0 = *( setA[ i ] );
				m.srcIdx = i;
				m.dstIdx = 0;
				m.distance = maxDescDistance;
				for( size_t k = 0; k < setB.size(); ++k ) {
					const T& d1 = setB[ k ];

					// euclidean distance
					float ptDist = ( d1.pt - d0.pt ).lengthSqr();
					if( ptDist > distanceSquare )
						continue;

					// descriptor distance
					float distance = dfunc( d0, d1 );
					if( distance < m.distance ) {
						m.dstIdx = k;
						m.distance = distance;
					}
				}

				if( m.distance < maxDescDistance )
					matches.push_back( m );
			}
		}

		template<typename T, typename DFUNC>
		static inline void scanLineMatch( std::vector<FeatureMatch>& matches,
										  const std::vector<const FeatureDescriptor*>& left,
										  const std::vector<T>& right,
										  DFUNC dfunc,
										  float minDisp,
										  float maxDisp,
										  float maxDescDist,
										  float maxLineDist )
		{
			matches.reserve( left.size() );
			FeatureMatch m;
			for( size_t i = 0; i < left.size(); ++i ){
				const FeatureDescriptor* d = left[ i ];
				m.distance = maxDescDist;
				m.feature0 = d;
				m.feature1 = 0;

				for( size_t k = 0; k < right.size(); ++k ){
					const FeatureDescriptor& dr = right[ k ];
					float yDist = Math::abs( d->pt[ 1 ] - dr.pt[ 1 ] );
					if( yDist < maxLineDist &&
						d->octave == dr.octave &&
						Math::abs( d->angle - dr.angle ) < 0.1f ){
						float disp = d->pt[ 0 ] - dr.pt[ 0 ];
						if( disp < minDisp && disp > maxDisp ){
							float descDist = dfunc( *d, dr );
							if( descDist < m.distance ){
								m.distance = descDist;
								m.feature1 = &dr;
							}
						}
					}
				}
				if( m.distance < maxDescDist ){
					matches.push_back( m );
				}
			}
		}
	}
}

#endif
