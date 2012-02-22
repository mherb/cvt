/*
			CVT - Computer Vision Tools Library

 	 Copyright (c) 2012, Philipp Heise, Sebastian Klose

 	THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 	KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 	IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 	PARTICULAR PURPOSE.
 */
#ifndef CLEXCEPTION_H
#define CLEXCEPTION_H
#include <cvt/cl/cl.hpp>
#include <cvt/util/Exception.h>

namespace cvt {

		class CLException : cvt::Exception
		{
			public:
				CLException( ) throw( ) : msg( "Unknown" ), log( "" ) {};
				CLException( std::string s ) throw( ) : msg( s ), log( "" ) {};
				CLException( cl_int error ) throw( );
				CLException( std::string location, cl_int error ) throw( );
				CLException( cl_int error, std::string logmessage ) throw( );
				~CLException( ) throw( ) {};

				const char* what( ) const throw( ) { return msg.c_str( ); };
				const char* getLog( ) const throw( ) { return log.c_str( ); };

			private:
				std::string msg;
				std::string log;
		};
}
#endif
