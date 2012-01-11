#include <cvt/geom/scene/Scene.h>

#include <iostream>

using namespace cvt;

int main( int argc, char** argv )
{
	if( argc != 2 ) {
		std::cout << argv[ 0 ] << " scene-file" << std::endl;
		return 1;
	}

	Scene s;
	s.load( argv[ 1 ] );

	std::cout << s << std::endl;
}
