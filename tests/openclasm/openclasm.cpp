#include <cvt/cl/CLContext.h>
#include <cvt/cl/CLKernel.h>
#include <iostream>
#include <fstream>
#include <sstream>

using namespace cvt;

int main( int argc, char** argv )
{
	if( argc != 3 ) {
		std::cout << "usage: " << argv[ 0 ] << " file.cl name" << std::endl;
		return 1;
	}

	std::ifstream fin( argv[ 1 ] );
	if( fin ) {
		std::string log;
		std::stringstream buffer;

		buffer << fin.rdbuf();
		fin.close();

//		std::cout << buffer.str() << std::endl;
		CLContext cl;
		cl.makeCurrent();
		CLKernel kern;
		kern.build( argv[ 2 ], buffer.str().c_str(), buffer.str().length(), log );
		std::cerr << "Compile log: " << log << std::endl;

		std::vector<char*>* bins = kern.getBinaries();
		for( std::vector<char*>::iterator it = bins->begin(); it != bins->end(); it++ ) {
			std::cout << ( *it ) << std::endl;
			delete *it;
		}
		delete bins;

	}
}
