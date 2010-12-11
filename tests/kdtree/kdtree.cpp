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
		uint32_t partition( uint32_t l, uint32_t h, int idx );

		/* select the x-th element according to dimension idx */
		void select(  uint32_t l, uint32_t h, uint32_t x, int idx );
		void print();

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
	//select( 0, _npts - 1, _npts << 1, 0 );
	print();
}

KDTree::~KDTree()
{
	delete[] _ptidx;
}

void KDTree::print()
{
	int idx = 0;
	for( uint32_t i = 0; i < _npts; i++ )
		std::cout << PT( i ) << " ";
	idx ^= 0x01;

	std::cout << std::endl;

	for( uint32_t i = 0; i < _npts; i++ )
		std::cout << PT( i ) << " ";
	std::cout << std::endl;
}

void KDTree::medsort( uint32_t l, uint32_t h, int idx )
{
	uint32_t med = ( h + l ) >> 1;

	std::cout << "medsort(" << med <<") " << l << " < - > " << h << " index: " << idx << std::endl;

	// partition
	select( l, h, med, idx );

	if( med > l + 2 )
		medsort( l, med - 1, idx ^ 0x01 );
	if( med < h - 2 )
		medsort( med + 1, h, idx ^ 0x01 );
}


void KDTree::select(  uint32_t l, uint32_t h, uint32_t x, int idx )
{
	if( l == h ) return;

	uint32_t i = partition( l, h, idx );
	uint32_t k = i - l ;

	if( x == k )
		return;
	else if( x < k )
		select( l, i - 1, x, idx );
	else
		select( i + 1, h, x - k, idx );
}

uint32_t KDTree::partition( uint32_t l, uint32_t h, int idx )
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

	// i defines is the partitioning index
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
	Point2f pts[ 11 ];

	for( int i = 0; i < 11; i++ )
		pts[ i ].set( Math::rand( 0.0f, 10.0f ), Math::rand( 0.0f, 10.0f ) );

	KDTree kdtree( pts, 11 );
	return 0;
}
