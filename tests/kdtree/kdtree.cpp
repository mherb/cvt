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

	//medsort( 0, _npts - 1, 0 );
	medsort2( 0, _npts - 1, npts >> 1, 0 );
	std::cout  << "final: " << std::endl;
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
	if( _h <= _l )
		return;

	uint32_t l = _l;
	uint32_t h = _h;
	uint32_t p;

	while( 1 ) {
		if( h <= l ) {
			medsort2( _l, med - 1, ( _l + med - 1 ) >> 1, idx ^ 0x01 );
			medsort2( med + 1, _h, ( _h + med + 1 ) >> 1, idx ^ 0x01 );
			return;
		} else {
			uint32_t mid = ( l + h ) >> 1;

			if( PT( l ) > PT( mid ) )
				SWAP( l, mid  );
			if( PT( mid ) > PT( h ) )
				SWAP( mid, h );
			if( PT( l ) > PT( mid ) )
				SWAP( l, mid  );
			SWAP( mid, h );
			p = h;
			h--;
			while( 1 ) {
				do l++; while( PT( l ) < PT( p ) );
				do h--; while( PT( h ) > PT( p ) );
				if( l >= h) break;
				SWAP( l, h );
			}
			SWAP( l , p);
			h++;
			if( med < l - 1 ) h = l - 1;
			else if( med > l + 1 ) l = l + 1;
			return;
		}
	}
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
	std::cout << "Partitioned: low = " << l << ", high = " << h << ", index of partitioning element: " << q << std::endl;

	if( l == 11 ){
		std::cout << std::endl;
		print();
		std::cout << std::endl;
	}


	if( i == q )
		return;
	else if( i < q )
		select( l, q - 1, i, idx );
	else
		select( q + 1, h, i, idx );
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
	std::cout << "SELECTED MEDIAN at index " << h << ", " << PT( h ) << std::endl;;

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
	Point2f pts[ 21 ];

	for( int i = 0; i < 21; i++ )
		pts[ i ].set( Math::rand( 0.0f, 10.0f ), Math::rand( 0.0f, 10.0f ) );

	KDTree kdtree( pts, 21 );
	return 0;
}
