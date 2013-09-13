/*
			CVT - Computer Vision Tools Library

 	 Copyright (c) 2012, Philipp Heise, Sebastian Klose

 	THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 	KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 	IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 	PARTICULAR PURPOSE.
 */
#include <iostream>
#include <cstring>
#include <exception>
#include <cvt/util/Exception.h>
#include <cvt/util/cvttestsproto.h>

static bool cvttest_run( CVTTestFunc func, const char* name )
{
    bool ret;

    std::cout <<"Running " << name << std::endl;
    std::cout << "-------------------------------------------------------------------------------" << std::endl;
    ret = func( );
    std::cout << "-------------------------------------------------------------------------------" << std::endl;
    std::cout << name << " returned: " << (ret?"TRUE":"FALSE") << std::endl;
    return ret;
}

int main( int argc, char** argv )
{
    size_t i = 0;
    size_t nsuccess = 0;

	if( argc == 1 ||  ( argc == 2 && strcmp( argv[1], "all" ) == 0 ) ) {
		while( _tests[i].func != NULL ) {
			try {
				if( cvttest_run( _tests[i].func, _tests[i].name ) )
					nsuccess++;
			} catch( cvt::Exception e ) {
				std::cerr << "Exception:" << std::endl;
				std::cerr << e.what() << std::endl;
			}

			i++;
		}
		std::cout << nsuccess << " PASSED and " << i - nsuccess << " FAILED of " << i << " TESTS" << std::endl;

	} else if( argc == 2 && strcmp( argv[1], "list" ) == 0 ) {
		while( _tests[i].func != NULL ) {
			std::cout << _tests[ i ].name << std::endl;
			i++;
		}
	} else {
        if( argc == 2 ) {
            size_t len;
            len = strlen( argv[ 1 ] );
            while( _tests[i].func != NULL ) {
                if( strncmp( _tests[i].name, argv[1], len ) == 0 ) {
					try {
						if( cvttest_run( _tests[i].func, _tests[i].name ) )
							nsuccess++;
					} catch( cvt::Exception e ) {
						std::cerr << "Exception:" << std::endl;
						std::cerr << e.what() << std::endl;
					}
                }
                i++;
            }
        }
    }

    return 0;
}

