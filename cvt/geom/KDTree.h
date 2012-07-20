/*
			CVT - Computer Vision Tools Library

 	 Copyright (c) 2012, Philipp Heise, Sebastian Klose

 	THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 	KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 	IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 	PARTICULAR PURPOSE.
 */
#ifndef CVT_KDTREE_H
#define CVT_KDTREE_H

#include <vector>

#include <cvt/math/Vector.h>

namespace cvt
{

#define PT( n ) _pts[ _ptidx[ n ] ][ idx ]
#define SWAP( a, b ) do { uint32_t t = _ptidx[ a ]; _ptidx[ a ] = _ptidx[ b ]; _ptidx[ b ] = t; } while( 0 )

        template<class _T=Point2f>
	class KDTree {
		public:
			KDTree( const std::vector<_T> & pts );
			~KDTree();

			// return index of nearest neighbor
			uint32_t locate( const _T & pt, float dist );
                        void rangeSearch( std::vector<_T> &output, const _T &pt, float dist );

		private:
                        void build( int32_t l, int32_t h, uint32_t idx );
                        uint32_t partition( int32_t l, int32_t h, uint32_t x, uint32_t idx );
			void medsort( uint32_t l, uint32_t h, uint32_t med, int idx );

			/* select the x-th element according to dimension idx */
            void select(  int32_t l, int32_t h, int32_t x, uint32_t idx );
			void print();
            void check( int32_t l, int32_t h, uint32_t idx );

            uint32_t visit( uint32_t low, uint32_t high, const _T & pt, float distance, uint32_t idx );
                        void visit( std::vector<_T> &output, const _T &pt, double *min, double *max, double distance, int low, int high, int idx );

			const std::vector<_T> & _pts;
			uint32_t* _ptidx;
                        uint32_t _dim;
	};

    template <class _T>
    inline KDTree<_T>::KDTree( const std::vector<_T> & pts ) : _pts( pts )
    {
        uint32_t npts = _pts.size();
        if( npts == 0 )
            return;
        _ptidx = new uint32_t[ npts ];
        _dim = pts[0].dimension();

        for( size_t i = 0; i < npts; i++ )
            _ptidx[ i ] = i;

        build( 0, npts - 1, 0 );
        check( 0, npts - 1, 0 );
    }

    template<class _T>
    inline KDTree<_T>::~KDTree()
    {
        delete[] _ptidx;
    }

    template <class _T>
    inline uint32_t KDTree<_T>::locate( const _T & pt, float dist )
    {
        uint32_t nearestIndex = visit( 0, _pts.size() - 1, pt, dist, 0 );

        return _ptidx[ nearestIndex ];
    }

    template <class _T>
    inline void KDTree<_T>::build( int32_t l, int32_t h, uint32_t idx )
    {
        if (l<h)
        {
            uint32_t med = (l+h)>>1;
            select( l, h, med, idx );

            build( l, med-1, (idx+1)%_dim );
            build( med+1, h, (idx+1)%_dim );
        }
    }

    template <class _T>
    inline uint32_t KDTree<_T>::partition( int32_t l, int32_t h, uint32_t x, uint32_t idx )
    {
        int32_t pivot = l;

        SWAP( x, h );

        for ( int32_t i = l; i < h; ++i )
        {
            if ( PT(i) < PT(h) )
            {
                SWAP( pivot, i );
                pivot++;
            }
        }

        SWAP( pivot, h );
        return pivot;
    }

    template <class _T>
    void KDTree<_T>::select( int32_t l, int32_t h, int32_t x, uint32_t idx )
    {
        if (h>l)
        {
            int32_t pivot = partition( l, h, x, idx );

            if ( pivot > x )
                select( l, pivot-1, x, idx );
            if ( pivot < x )
                select( pivot+1, h, x, idx );
        }
    }

    template <class _T>
    inline uint32_t KDTree<_T>::visit( uint32_t low, uint32_t high,
                                       const _T & pt, float dist, uint32_t idx )
    {
        if( high - low < 2 ){
            // check low and high
            float nearest = ( _pts[ _ptidx[ low ] ] - pt ).length();
            if( nearest <  ( _pts[ _ptidx[ high ] ] - pt ).length() )
                return low;
            else
                return high;
        }

        uint32_t medianIdx = ( high + low ) >> 1;

        float min = pt[ idx ] - dist;
        float max = pt[ idx ] + dist;

        if( PT( medianIdx ) > max ){
            // search left half
            return visit( low, medianIdx - 1, pt, dist, (idx+1)%_dim );
        }

        if( PT( medianIdx ) < min ){
            // search right half
            return visit( medianIdx + 1, high, pt, dist, (idx+1)%_dim );
        }

        // search both halfes and compare the results and also the median!:
        uint32_t bestLow = visit( low, medianIdx - 1, pt, dist, (idx+1)%_dim );
        uint32_t bestHigh = visit( medianIdx + 1, high, pt, dist, (idx+1)%_dim );

        const _T & other = _pts[ _ptidx[ medianIdx ] ];
        uint32_t bestIdx = medianIdx;
        float nearestDistance = ( other - pt ).length();

        float tmp = ( _pts[ _ptidx[ bestLow ] ] - pt ).length();
        if( tmp < nearestDistance ){
            nearestDistance = tmp;
            bestIdx = bestLow;
        }

        tmp = ( _pts[ _ptidx[ bestHigh ] ] - pt ).length();
        if( tmp < nearestDistance ){
            nearestDistance = tmp;
            bestIdx = bestHigh;
        }

        return bestIdx;
    }

    template<class _T>
    void KDTree<_T>::rangeSearch( std::vector<_T> &output, const _T &pt, float distance )
    {
        double min[ _dim ];
        double max[ _dim ];

        for( uint32_t i = 0; i < _dim; ++i ){
            min[i] = pt[i] - distance;
            max[i] = pt[i] + distance;
        }

        visit( output, pt, min, max, distance, 0, _pts.size()-1, 0 );
    }

    template<class _T>
    void KDTree<_T>::visit( std::vector<_T> &output, const _T &pt, double *min, double *max, double distance, int32_t l, int32_t h, int idx )
    {
        uint32_t med = (l+h) >> 1;
        const _T &root = _pts[ _ptidx[ med ] ];

        if (h >= l)
        {
            float dist = (root-pt).length();

            if ( dist <= distance )
            {
                output.push_back(root);
            }
        }

        if (h > l)
        {
            if (max[idx] >= root[idx])
                visit( output, pt, min, max, distance, med+1, h, (idx+1)%_dim );

            if (min[idx] <= root[idx])
                visit( output, pt, min, max, distance, l, med-1, (idx+1)%_dim );
        }
    }

    template<class _T>
    inline void KDTree<_T>::print()
    {
        for( int idx = 0; idx < _dim; ++idx ) {
            printf( "dim %d : ", idx );
            for( uint32_t i = 0; i < _pts.size(); i++ ){
                std::cout << PT( i ) << " ";
            }
            printf( "\n\n" );
        }
    }

    template<class _T>
    inline void KDTree<_T>::medsort( uint32_t _l, uint32_t _h, uint32_t med, int idx )
    {
        uint32_t l = _l;
        uint32_t h = _h;
        uint32_t p;

        if( h <= l + 1 )
            return;

        while( 1 ) {
            if( h <= l ) {
                medsort( _l, med - 1, ( _l + med - 1 ) >> 1, (idx+1)%_dim );
                medsort( med + 1, _h, ( _h + med + 1 ) >> 1, (idx+1)%_dim );
                return;
            } else {
                uint32_t mid = ( l + h ) >> 1;
                uint32_t i, k;

                if( PT( l ) > PT( mid ) ){
                    SWAP( l, mid  );
                }
                if( PT( mid ) > PT( h ) )
                    SWAP( mid, h );
                if( PT( l ) > PT( mid ) )
                    SWAP( l, mid  );
                SWAP( mid, h );

                p = h;
                h--;
                i = l;
                k = h;
                while( 1 ) {
                    while( PT( i ) < PT( p ) && i < k ) i++;
                    while( PT( k ) >= PT( p ) && k ) k--;
                    if( i >= k ) break;
                    SWAP( i, k );
                }
                SWAP( i , p);
                h++;

                if( med < i ) h = i;
                else if( med > i ) l = ++i;
                else { h = l; };
            }
        }
    }

    template<class _T>
    inline void KDTree<_T>::check( int32_t l, int32_t h, uint32_t idx )
    {
        int32_t med = ( l + h ) >> 1;

        if( l >= h )
            return;

        for( int32_t x = l; x < h; x++ ) {
            if( x < med && PT( x ) > PT( med ) )
                std::cout << "lower than median has bigger value" << std::endl;
            if( x > med && PT( x ) < PT( med ) )
                std::cout << "bigger than median has lower value" << std::endl;
        }
        check( l, med - 1, (idx+1)%_dim );
        check( med + 1, h, (idx+1)%_dim );
    }

}

#endif
