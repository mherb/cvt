#ifndef CVT_KDTREE_H
#define CVT_KDTREE_H

#include <vector>

#include <cvt/math/Vector.h>

namespace cvt
{
	class KDTree {
		public:
			KDTree( Point2f* pts, size_t npts );
			~KDTree();

			// return index of nearest neighbor
			uint32_t locate( Point2f* pt, float dist );

		private:
			void medsort( uint32_t l, uint32_t h, uint32_t idx );
			uint32_t partition( uint32_t l, uint32_t h, uint32_t idx );
			void medsort2( uint32_t l, uint32_t h, uint32_t med, int idx );

			/* select the x-th element according to dimension idx */
			void select(  uint32_t l, uint32_t h, uint32_t x, uint32_t idx );
			void print();
			void check( uint32_t l, uint32_t h, uint32_t idx );

			uint32_t visit( uint32_t low, uint32_t high, Point2f * pt, float dist, uint32_t idx );

			Point2f* _pts;
			uint32_t* _ptidx;
			size_t _npts;
	};
}

#endif
