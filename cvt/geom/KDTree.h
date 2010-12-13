#ifndef CVT_KDTREE_H
#define CVT_KDTREE_H

#include <vector>

#include <cvt/math/Vector.h>

namespace cvt
{

#define PT( n ) _pts[ _ptidx[ n ] ][ idx ]
#define SWAP( a, b ) do { uint32_t t = _ptidx[ a ]; _ptidx[ a ] = _ptidx[ b ]; _ptidx[ b ] = t; } while( 0 )

	template <class _T = Point2f>
	class KDTree {
		public:
			KDTree( const std::vector<_T> & pts );
			~KDTree();

			// return index of nearest neighbor
			uint32_t locate( const _T & pt, float dist );

		private:
			uint32_t partition( uint32_t l, uint32_t h, uint32_t idx );
			void medsort( uint32_t l, uint32_t h, uint32_t med, int idx );

			/* select the x-th element according to dimension idx */
			void select(  uint32_t l, uint32_t h, uint32_t x, uint32_t idx );
			void print();
			void check( uint32_t l, uint32_t h, uint32_t idx );

			uint32_t visit( uint32_t low, uint32_t high, const _T & pt, float dist, uint32_t idx );

			const std::vector<_T> & _pts;
			uint32_t* _ptidx;
	};

	template <class _T>
	inline KDTree<_T>::KDTree( const std::vector<_T> & pts ) : _pts( pts )
	{
		uint32_t npts = _pts.size();
		_ptidx = new uint32_t[ npts ];

		for( size_t i = 0; i < npts; i++ )
			_ptidx[ i ] = i;

		medsort( 0, npts - 1, npts >> 1, 0 );
		//check( 0, _npts - 1, 0 );
	}

	template <class _T>
	inline uint32_t KDTree<_T>::locate( const _T & pt, float dist )
	{
		uint32_t nearestIndex = visit( 0, _pts.size() - 1, pt, dist, 0 );

		return _ptidx[ nearestIndex ];
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
			return visit( low, medianIdx - 1, pt, dist, idx^0x01 );
		}

		if( PT( medianIdx ) < min ){
			// search right half
			return visit( medianIdx + 1, high, pt, dist, idx^0x01 );
		}

		// search both halfes and compare the results and also the median!:
		uint32_t bestLow = visit( low, medianIdx - 1, pt, dist, idx^0x01 );
		uint32_t bestHigh = visit( medianIdx + 1, high, pt, dist, idx^0x01 );

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



}

#endif
