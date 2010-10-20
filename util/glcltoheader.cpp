#include <iostream>
#include <fstream>
#include <string>

int main( int argc, char** argv )
{
    char val;

    if( argc != 4 ) {
	std::cerr << "usage: " << argv[ 0 ] << "cl-file header-file name" << std::endl;
	return 1;
    }

    std::ifstream input;
    input.open( argv[ 1 ] );
    if( !input.is_open() ) {
	std::cerr << "Unable to open cl-file!" << std::endl;
	return 1;
    }
    std::ofstream output;
    output.open( argv[ 2 ], std::ios::trunc );
    if( !output.is_open() ) {
	std::cerr << "Unable to open header-file!" << std::endl;
	return 1;
    }


    output << "#ifndef CL_" << argv[ 3 ] << "_H" << std::endl;
    output << "#define CL_" << argv[ 3 ] << "_H" << std::endl;
    output << "static const char _" << argv[ 3 ] << "_source[ ] =" << std::endl;
    output << "\"";
    while( !input.eof() ) {
	input.get( val );
	if( !input.eof() ) {
	    switch( val ) {
		case '\n': output << "\\n\" \\"  << std::endl << "\"";
			   break;
		case '\"': output << "\\\"";
			   break;
		default: output << val;
	    }
	}
    }
    output << "\\n\";" << std::endl;
    output << "#endif" << std::endl;
    input.close();
    output.close();

    return 0;
}
