#include <cvt/util/ConfigFile.h>

using namespace cvt;

int main( int argc, char* argv[] )
{
	if( argc != 2 ){
		std::cout << "Usage: " << argv[ 0 ] << " <configfile>" << std::endl;
		return 0;
	}

	ConfigFile cfg( argv[ 1 ] );

	/*
		1) train predictor for center of picture
		2) track motion
		3) remove regions at border
		4) add sample points/subpredictors at regions with few points
	/*

	return 0;
}
