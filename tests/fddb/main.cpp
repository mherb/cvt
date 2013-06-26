#include "FDDB.h"

using namespace cvt;

int main( int argc, char** argv )
{
	FDDB fddb( argv[ 1 ], argv[ 2 ] );
	std::cout << fddb.size() << std::endl;



	FDDB filtered;
	for( float min = 0; min < 20000; min += 1000 ) {
		float amaj, amin;
		filtered.clear();
		fddb.filterArea( filtered, min, min + 1000.0f );
		filtered.averageSemiMajorMinor( amaj, amin );
		std::cout << min << " - " << min + 1000 << " : " << filtered.size() << " ( " << ( int )( filtered.size() * 100.0f / fddb.size() + 0.5f ) << "% )  " << filtered.averageArea() << " " << amaj << " " << amin << std::endl;
	}

		float amaj, amin;
		filtered.clear();
		fddb.filterArea( filtered, 5000.0f, 8000.0f );
		filtered.averageSemiMajorMinor( amaj, amin );
		std::cout << " 5000 - 8000 " << " : " << filtered.size() << " ( " << ( int )( filtered.size() * 100.0f / fddb.size() + 0.5f ) << "% )  " << filtered.averageArea() << " " << amaj << " " << amin << std::endl;

}
