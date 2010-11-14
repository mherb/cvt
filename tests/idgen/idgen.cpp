#include <cvt/util/IDGenerator.h>
#include <iostream>


using namespace cvt;


int main()
{
    IDGenerator idgen;

#define SIZE 2000

    uint32_t id[ SIZE ];

    for( int i = 0; i < SIZE ; i++ ) {
	id[ i ] = idgen.allocateID();
	std::cout << "GOT " << id[ i ] << std::endl;
    }
    for( int i = SIZE - 1; i >= 0; i-- ) {
	idgen.releaseID( id[ i ] );
    }
    for( int i = 0; i < SIZE; i++ ) {
	id[ i ] = idgen.allocateID();
	std::cout << "GOT " << id[ i ] << std::endl;
    }

/*    idgen.releaseID( 1 );
    id[ 20 ] = idgen.allocateID();
    std::cout << "GOT " << id[ 20 ] << std::endl;
    idgen.releaseID( 20 );
    id[ 21 ] = idgen.allocateID();
    std::cout << "GOT " << id[ 21 ] << std::endl;*/


}
