#include <iostream>

#include <cvt/geom/KDTree.h>
#include <cvt/math/Vector.h>


using namespace cvt;


int main()
{
#define SIZE 11

	Point2f pts[ SIZE ];

	for( int i = 0; i < SIZE; i++ )
		pts[ i ].set( Math::rand( 0.0f, 10.0f ), Math::rand( 0.0f, 10.0f ) );

	std::cout << "Original array:\nx:\t";
	for( int i = 0; i < SIZE; i++ ){
		std::cout << pts[ i ][ 0 ] << "\t";
	}
	std::cout << "\ny:\t";
	for( int i = 0; i < SIZE; i++ ){
		std::cout << pts[ i ][ 1 ] << "\t";
	}
	std::cout << std::endl;

	KDTree kdtree( pts, SIZE );

	Point2f p( 5.1, 3.5 );
	uint32_t index = kdtree.locate( &p, 0.2f );
	std::cout << "NEAREST NEIGHBOR INDEX: " << index << ": Point ( " << pts[ index ][ 0 ] << ", " << pts[ index ][ 1 ] << " )" << std::endl;

	return 0;
}
