#include <iostream>

#include <cvt/util/String.h>
#include <cvt/gui/Application.h>

#include "MetaioData.h"
#include "MetaioGUI.h"

using namespace cvt;


int main( int argc, char* argv[] )
{

	if( argc < 3 ){
		std::cout << "Usage: " << argv[ 0 ] << "<folder> <sequence-nr>" << std::endl;
		return 0;
	}

	String folder( argv[ 1 ] );
	String sequence( argv[ 2 ] );

	MetaioData data( folder, ( size_t )sequence.toInteger() );
	MetaioGUI gui( data );

	Application::run();

	return 0;
}
