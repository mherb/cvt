#include <cvt/geom/scene/Scene.h>

#include <iostream>

using namespace cvt;

int main( int argc, char** argv )
{
	if( argc != 2 ) {
		std::cout << argv[ 0 ] << " ply-file" << std::endl;
		return 1;
	}

	Scene s;
	s.load( argv[ 1 ] );

	std::cout << s << std::endl;

/*	Model mdl;
	PlyModel::load( mdl, argv[ 1 ] );
	std::cout << mdl << std::endl;

//	mdl.calcNormals();
	PlyModel::save( "output.ply", mdl ); */
}
