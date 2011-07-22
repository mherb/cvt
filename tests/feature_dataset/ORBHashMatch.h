#ifndef CVT_ORBHASHMATH_H
#define CVT_ORBHASHMATH_H

#include <cvt/vision/ORB.h>
#include <cvt/util/List.h>

namespace cvt {

#define MAXIDX 16

	class ORBHashMatch {
		public:
			ORBHashMatch( const ORB& orb );
			int find( const ORBFeature& feature, size_t& dist, size_t maxDistance = 30 );

		private:
			void hashORBFeatures();

			const ORB& _orb;
			List<int> _htable[ MAXIDX ][ 256 ];
	};

	inline ORBHashMatch::ORBHashMatch( const ORB& orb ) : _orb( orb )
	{
		hashORBFeatures();
	}

	inline void ORBHashMatch::hashORBFeatures( )
	{
		for( size_t i = 0, end = _orb.size(); i < end; i++ ) {
			const ORBFeature& feature = _orb[ i ];
			for( size_t idx = 0; idx < MAXIDX; idx++ ) {
				_htable[ idx ][ feature.desc[ idx ] ].append( i );
			}
		}
/*		for( size_t k = 0; k < MAXIDX; k++ ) {
			for( size_t l = 0; l < 256; l++ ) {
				std::cout << k << " " << l << " " << _htable[ k ][ l ].size() << std::endl;
			}
		}*/
	}

	inline int ORBHashMatch::find( const ORBFeature& feature, size_t& dist, size_t maxDistance )
	{
		int ret = -1;
		size_t retdist = maxDistance;

		/* Simple test without any descriptor bit twiddling */

		for( size_t idx = 0; idx < MAXIDX; idx++ ) {
			int index = feature.desc[ idx ];
			for( List<int>::Iterator it = _htable[ idx ][ index ].begin(), end = _htable[ idx ][ index ].end(); it != end; ++it ){
				const ORBFeature& current = _orb[ *it ];
				size_t d = feature.distance( current );
				if( d < retdist ) {
					retdist = d;
					ret = *it;
				}
			}
		}
		dist = retdist;
		return ret;
	}
}

#endif
