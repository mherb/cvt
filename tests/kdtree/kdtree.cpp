#include <iostream>

//#include <cvt/geom/KDTree.h>

#include <cvt/math/Vector.h>

using namespace cvt;

typedef Vector2f Point2f;

#define PT( n ) _pts[ _ptidx[ n ] ][ idx ] 
#define SWAP( a, b ) do { uint32_t t = _ptidx[ a ]; _ptidx[ a ] = _ptidx[ b ]; _ptidx[ b ] = t; } while( 0 )


class KDTree {
	public:
		KDTree( Point2f* pts, size_t npts );
		~KDTree();
		void locate( Point2f* pt, float dist );
	private:
		void medsort( uint32_t l, uint32_t h, int idx );

		Point2f* _pts;
		uint32_t* _ptidx;
		size_t _npts;
};


KDTree::KDTree( Point2f* pts, size_t npts )
	: _pts( pts ), _npts( npts )
{
	_ptidx = new uint32_t[ _npts ];

	for( size_t i = 0; i < _npts; i++ )
		_ptidx[ i ] = i;
	medsort( 0, _npts - 1, 0 );

	{
		int idx = 0;
		for( int i = 0; i < _npts; i++ )
			std::cout << PT( i ) << " ";
		std::cout << std::endl;
	}
}

KDTree::~KDTree()
{
	delete[] _ptidx;
}

void KDTree::medsort( uint32_t l, uint32_t h, int idx )
{
	uint32_t ll = l;
	uint32_t hh = h;
	uint32_t med = ( h + l ) >> 1;

	std::cout << "medsort" << l << " < - > " << h << std::endl;
	if( h == l || l == h - 1 )
		return;

	while( 1 ) {
		if( hh  <= ll + 1 ) {
			medsort( l, med - 1, idx ^ 0x01 );
			medsort( med + 1, h, idx ^ 0x01 );
			return;
		} else {
			uint32_t mid = ( hh + ll ) >> 1;
			if( PT( ll ) > PT( mid ) )
				SWAP( ll, mid  );
			if( PT( mid ) > PT( hh ) )
				SWAP( mid, hh );
			if( PT( ll ) > PT( mid ) )
				SWAP( ll, mid  );
			SWAP( mid, hh );

			uint32_t i, k;
			uint32_t p = hh;
			hh--;

			i = hh;
			k = ll;
			std::cout << PT( ll ) << " " << PT( mid ) << " " << PT( hh + 1 ) << std::endl;

			while( 1 ) {
				while( PT( i ) > PT( p ) ) i--;
				while( PT( k ) < PT( p ) ) k++;
				if( i <= k )
					break;
				SWAP( i, k );
			}

			SWAP( k, p );
			if( i >= med  ) { hh = i; }
			else { ll = k; h++; }
		}
	}
}



int main()
{
	Point2f pts[ 11 ];

	for( int i = 0; i < 10; i++ )
		pts[ i ].set( Math::rand( 0.0f, 10.0f ), Math::rand( 0.0f, 10.0f ) );

	KDTree kdtree( pts, 10 );
	return 0;
}
