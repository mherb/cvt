#include <cvt/geom/KDTree.h>

namespace cvt
{
	template <>
	KDTree<>::~KDTree()
	{
		delete[] _ptidx;
	}


	template<>
	void KDTree<>::print()
	{
		int idx = 0;
		std::cout << "x : ";
		for( uint32_t i = 0; i < _pts.size(); i++ )
			std::cout << PT( i ) << " ";

		std::cout << std::endl;
		idx ^= 0x01;
		std::cout << "y : ";
		for( uint32_t i = 0; i < _pts.size(); i++ )
			std::cout << PT( i ) << " ";
		std::cout << std::endl;
	}

	template<>
	void KDTree<>::medsort( uint32_t _l, uint32_t _h, uint32_t med, int idx )
	{
		uint32_t l = _l;
		uint32_t h = _h;
		uint32_t p;


		if( h <= l + 1 )
			return;

		while( 1 ) {
			if( h <= l ) {
				medsort( _l, med - 1, ( _l + med - 1 ) >> 1, idx ^ 0x01 );
				medsort( med + 1, _h, ( _h + med + 1 ) >> 1, idx ^ 0x01 );
				return;
			} else {
				uint32_t mid = ( l + h ) >> 1;
				uint32_t i, k;

				if( PT( l ) > PT( mid ) )
					SWAP( l, mid  );
				if( PT( mid ) > PT( h ) )
					SWAP( mid, h );
				if( PT( l ) > PT( mid ) )
					SWAP( l, mid  );
				SWAP( mid, h );

				p = h--;
				i = l;
				k = h;
				while( 1 ) {
					while( PT( i ) < PT( p ) ) i++;
					while( PT( k ) > PT( p ) ) k--;
					if( i >= k) break;
					SWAP( i, k );
				}
				SWAP( i , p);
				h++;

				if( med < i ) h = i;
				else if( med > i ) l = i;
				else { h = l; };
			}
		}
	}

	template<>
	void KDTree<>::check( uint32_t l, uint32_t h, uint32_t idx )
	{
		uint32_t med = ( l + h ) >> 1;

		if( h >= l )
			return;

		for( uint32_t x = l; x < h; x++ ) {
			if( x < med && PT( x ) > PT( med ) )
				std::cout << "lower than median has bigger value" << std::endl;
			if( x > med && PT( x ) < PT( med ) )
				std::cout << "lower than median has bigger value" << std::endl;
		}
		check( l, med - 1, idx ^ 0x01 );
		check( med + 1, h, idx ^ 0x01 );
	}
}
