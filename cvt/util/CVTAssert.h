#ifndef CVTASSERT_H
#define CVTASSERT_H


#ifdef DEBUG
#define CVT_ASSERT( a, msg ) do { if( !( a ) ) { std::cerr << "Assertion "#a" failed ( " << __FILE__ << " : " << __LINE__ << " : " << __PRETTY_FUNCTION__ << " ) : "  << msg << std::endl; } } while( 0 )
#else
#define CVT_ASSERT( a, msg )
#endif

#endif

