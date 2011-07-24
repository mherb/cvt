/*
 * File:   LSH.h
 * Author: sebi
 *
 * Created on July 22, 2011, 3:56 PM
 */

#ifndef LSH_H
#define	LSH_H

#include <cvt/vision/ORB.h>

#include <algorithm>
#include <list>

namespace cvt {

    #define MAXIDX 16

    template <size_t NumBits = 11>
    class LSH {
      public:
        LSH( const ORB& orb );
        int find( const ORBFeature& feature, size_t& dist, size_t maxDistance = 30 );

      private:
        struct HistPos
        {
            int accum;
            size_t pos;
            static int halfOrbSize;

            bool operator<( const HistPos & other ) const
            {
                return ::abs( accum * 2 - halfOrbSize ) <= ::abs( other.accum * 2 - halfOrbSize );
            }
        };

        void hashORBFeatures();

        void entropy();

        size_t hash( const ORBFeature & ft ) const;

        const ORB& _orb;

        size_t _bitPos[ NumBits ];

        std::list<int> _htable[ ( 1 << NumBits ) ];
    };

    template<size_t NumBits>
    int LSH<NumBits>::HistPos::halfOrbSize;

    template <size_t NumBits>
    inline LSH<NumBits>::LSH( const ORB& orb ) : _orb( orb )
    {
        entropy();
        hashORBFeatures();
    }

    #define GETBIT( ft, n ) ( ( ft.desc[ n >> 3 ] >> ( n & 0x07 ) ) & 1 )
    template <size_t NumBits>
    inline void LSH<NumBits>::entropy()
    {
        std::vector<HistPos> hist( 256 );

        HistPos::halfOrbSize = _orb.size();

        for( size_t i = 0; i < 256; i++ ){
            hist[ i ].accum = 0;
            hist[ i ].pos = i;
        }

        for ( size_t i = 0, end = _orb.size( ); i < end; i++ ) {
            const ORBFeature& feature = _orb[ i ];

            for( size_t b = 0; b < 0xFF; b++ ){
                if( GETBIT( feature, b ) )
                    hist[ b ].accum++;
            }
        }

        std::sort( hist.begin(), hist.end() );

        // save the NumBits best bit positions for hash generation
        for( size_t i = 0; i < NumBits; i++ ){
            _bitPos[ i ] = hist[ i ].pos;
        }

        /*
        for( size_t b = 0; b < 0xFF; b++ ){
            std::cout << ( float )hist[ b ].accum / ( float )_orb.size()  << " " << hist[ b ].pos << " acuum: " << hist[ b ].accum << " abs:"<< ::abs( hist[ b ].accum * 2 - HistPos::halfOrbSize ) <<"\n";
        }
        std::cout << std::endl;
         */
    }

    template <size_t NumBits>
    inline size_t LSH<NumBits>::hash( const ORBFeature & ft ) const
    {
        size_t ret = 0;

        for( size_t i = 0; i < NumBits; i++ ){
            ret |= GETBIT( ft, _bitPos[ i ] ) << i;
        }

        return ret;
    }


    template <size_t NumBits>
    inline void LSH<NumBits>::hashORBFeatures()
    {
        for ( size_t i = 0, end = _orb.size( ); i < end; i++ ) {
            const ORBFeature& feature = _orb[ i ];
			size_t idx = hash( feature );
            _htable[ idx ].push_back( i );
        }

        /*
        for( size_t i = 0; i < ( 1 << NumBits ) - 1; i++ ){
            std::cout << _htable[ i ].size() << std::endl;
        }
         */


        /*		for( size_t k = 0; k < MAXIDX; k++ ) {
                    for( size_t l = 0; l < 256; l++ ) {
                        std::cout << k << " " << l << " " << _htable[ k ][ l ].size() << std::endl;
                    }
                }*/
    }

    template <size_t NumBits>
    inline int LSH<NumBits>::find( const ORBFeature& feature, size_t& dist, size_t maxDistance )
    {
        int ret = -1;
        size_t retdist = maxDistance;

        /* Simple test without any descriptor bit twiddling */
        size_t idx = hash( feature );

        for ( std::list<int>::iterator it = _htable[ idx ].begin( ), end = _htable[ idx ].end( ); it != end; ++it ) {
            const ORBFeature& current = _orb[ *it ];
            size_t d = feature.distance( current );
            if ( d < retdist ) {
                retdist = d;
                ret = *it;
            }
        }

		/* toggle one bit at each position and probe */
		for( size_t i = 0; i < NumBits; i++ ) {
			size_t index = idx ^ ( 1 << i );
			for ( std::list<int>::iterator it = _htable[ index ].begin( ), end = _htable[ index ].end( ); it != end; ++it ) {
				const ORBFeature& current = _orb[ *it ];
				size_t d = feature.distance( current );
				if ( d < retdist ) {
					retdist = d;
					ret = *it;
				}
			}
		}

        dist = retdist;
        return ret;
    }
}


#endif	/* LSH_H */

