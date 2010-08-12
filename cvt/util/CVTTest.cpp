#include <iostream>
#include <cstring>

#include "cvttestsproto.h"

static bool cvttest_run( ZTestFunc func, const char* name )
{
    bool ret;

    std::cout <<"Running " << name << std::endl;
    std::cout << "-------------------------------------------------------------------------------" << std::endl;
    ret = func( );
    std::cout << "-------------------------------------------------------------------------------" << std::endl;
    std::cout << name << " returned: " << ret?"TRUE":"FALSE" << std::endl;
    return ret;
}

int main( int argc, char** argv )
{
    size_t i = 0;
    size_t nsuccess = 0;

    zsystem_init( argc, argv );


    if( argc == 1 ||  ( argc == 2 && strcmp( argv[1], "all" ) == 0 ) ) {
        while( tests[i].func != NULL ) {
            if( cvttest_run( tests[i].func, tests[i].name ) )
                nsuccess++;
            i++;
        }
	std::cout << nsuccess << " PASSED and " << i - nsuccess << " FAILED of " << i << " TESTS" << std::endl;

    } else {
        if( argc == 2 ) {
            size_t len;
            len = strlen( argv[ 1 ] );
            while( tests[i].func != NULL ) {
                if( strncmp( tests[i].name, argv[1], len ) == 0 ) {
                    if( cvttest_run( tests[i].func, tests[i].name ) )
                        nsuccess++;
                }
                i++;
            }
        }
    }

    return 0;
}

