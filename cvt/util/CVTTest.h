#ifndef CVTTEST_H
#define CVTTEST_H

#include <iostream>

typedef bool (*CVTTestFunc)( void );

typedef struct {
    CVTTestFunc func;
    const char* name;
} CVTTest;


#define BEGIN_CVTTEST(x) extern "C" { bool x##_test( void ) {
#define END_CVTTEST } }

#define CVTTEST_PRINT( str, result  )	do { char c = 0x1B; \
							if( ( result ) ) \
								std::cerr << str << " : " << c << "[1;32mOK" << c << "[0;m" << std::endl; \
							else \
								std::cerr << str << " : " << c <<"[1;31mFAILED" << c << "[0;m" << std::endl; \
							} while( 0 )

#endif
