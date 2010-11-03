#ifndef CVTASSERT_H
#define CVTASSERT_H

#ifdef DEBUG
#define CVTAssert( a, msg ) do { if( !( a ) ) { std::cout << "Assertion "#a" failed: " << msg << std::endl; } } while( 0 )
#else
#define CVTAssert
#endif

#endif

