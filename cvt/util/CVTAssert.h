/*
			CVT - Computer Vision Tools Library

 	 Copyright (c) 2012, Philipp Heise, Sebastian Klose

 	THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 	KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 	IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 	PARTICULAR PURPOSE.
 */
#ifndef CVTASSERT_H
#define CVTASSERT_H


#ifdef DEBUG
#define CVT_ASSERT( a, msg ) do { if( !( a ) ) { std::cerr << "Assertion "#a" failed ( " << __FILE__ << " : " << __LINE__ << " : " << __PRETTY_FUNCTION__ << " ) : "  << msg << std::endl; } } while( 0 )
#else
#define CVT_ASSERT( a, msg )
#endif

#endif

