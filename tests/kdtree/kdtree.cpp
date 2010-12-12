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
		void medsort( uint32_t l, uint32_t h, uint32_t idx );
		uint32_t partition( uint32_t l, uint32_t h, uint32_t idx );
		void medsort2( uint32_t l, uint32_t h, uint32_t med, int idx );

		/* select the x-th element according to dimension idx */
		void select(  uint32_t l, uint32_t h, uint32_t x, uint32_t idx );
		void print();
		void check( uint32_t l, uint32_t h, uint32_t idx );

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

	std::cout  << "initial: " << std::endl;
	print();
	std::cout  << std::endl;

//	medsort( 0, _npts - 1, 0 );
	medsort2( 0, _npts - 1, npts >> 1, 0 );
	std::cout  << "final: " << std::endl;
	print();
	check( 0, _npts - 1, 0 );
}

KDTree::~KDTree()
{
	delete[] _ptidx;
}

void KDTree::print()
{
	int idx = 0;
	std::cout << "x : ";
	for( uint32_t i = 0; i < _npts; i++ )
		std::cout << PT( i ) << " ";

	std::cout << std::endl;
	idx ^= 0x01;
	std::cout << "y : ";
	for( uint32_t i = 0; i < _npts; i++ )
		std::cout << PT( i ) << " ";
	std::cout << std::endl;
}

void KDTree::medsort( uint32_t l, uint32_t h, uint32_t idx )
{
	uint32_t med = ( h + l ) >> 1;

	// partition
	select( l, h, med, idx );

	if( med > l + 1 )
		medsort( l, med - 1, idx ^ 0x01 );
	if( med < h - 1 )
		medsort( med + 1, h, idx ^ 0x01 );
}

void KDTree::medsort2( uint32_t _l, uint32_t _h, uint32_t med, int idx )
{
	uint32_t l = _l;
	uint32_t h = _h;
	uint32_t p;


	if( h <= l + 1 )
		return;
//	std::cout << "medsort(" << med <<") " << _l << " < - > " << _h << " index: " << idx << std::endl;

	while( 1 ) {
		if( h <= l ) {
//			std::cout << ( PT( med ) ) << std::endl;
			medsort2( _l, med - 1, ( _l + med - 1 ) >> 1, idx ^ 0x01 );
			medsort2( med + 1, _h, ( _h + med + 1 ) >> 1, idx ^ 0x01 );
			return;
		} /*else if( l + 1 == h ) {
			if( PT( l ) > PT( h ) )
				SWAP( l, h  );
			return;
		}*/ else {
			uint32_t mid = ( l + h ) >> 1;
			uint32_t i, k;

//			std::cout << l << " < - > " << h << std::endl;

			if( PT( l ) > PT( mid ) )
				SWAP( l, mid  );
			if( PT( mid ) > PT( h ) )
				SWAP( mid, h );
			if( PT( l ) > PT( mid ) )
				SWAP( l, mid  );
			SWAP( mid, h );
//			print();
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
//			print();
			if( med < i ) h = i;
			else if( med > i ) l = i;
			else { h = l; };
		}
	}
}

void KDTree::check( uint32_t l, uint32_t h, uint32_t idx )
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


void KDTree::select(  uint32_t l, uint32_t h, uint32_t i, uint32_t idx )
{
	if( l == h ) return;

	if( l == 11 ){
		std::cout << std::endl;
		print();
		std::cout << std::endl;
	}

	uint32_t q = partition( l, h, idx );
//	std::cout << "Partitioned: low = " << l << ", high = " << h << ", index of partitioning element: " << q << std::endl;

	if( l == 11 ){
		std::cout << std::endl;
		print();
		std::cout << std::endl;
	}


	if( i == q )
		return;
	else if( i < q )
		select( l, q , i, idx );
	else
		select( q , h, i, idx );
}

uint32_t KDTree::partition( uint32_t l, uint32_t h, uint32_t idx )
{
	// median of 3
	uint32_t mid = ( l + h ) >> 1;
	if( PT( l ) > PT( mid ) )
		SWAP( l, mid  );
	if( PT( mid ) > PT( h ) )
		SWAP( mid, h );
	if( PT( l ) > PT( mid ) )
		SWAP( l, mid  );
	SWAP( mid, h );
//	std::cout << "SELECTED MEDIAN at index " << h << ", " << PT( h ) << std::endl;;

	// i defines the partitioning index
	int32_t i = l - 1;
	for( uint32_t j = l ; j < h; j++ ){
		if( PT( j ) <= PT( h ) ){
			i++;
			SWAP( i, j );
		}
	}
	// MOVE our partitioning element back to the partitioning position
	i++;
	SWAP( i, h );

	return i;
}



int main()
{
#define SIZE 21

	Point2f pts[ SIZE ];

	for( int i = 0; i < SIZE; i++ )
		pts[ i ].set( Math::rand( 0.0f, 10.0f ), Math::rand( 0.0f, 10.0f ) );

	KDTree kdtree( pts, SIZE );
	return 0;
}
