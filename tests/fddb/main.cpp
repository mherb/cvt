#include "FDDB.h"

using namespace cvt;

int main( int argc, char** argv )
{
	FDDB fddb( argv[ 1 ], argv[ 2 ] );
	std::cout << fddb.size() << std::endl;


	std::cout << fddb.averageArea() << std::endl;
}
