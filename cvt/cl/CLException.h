#ifndef CLEXCEPTION_H
#define CLEXCEPTION_H
#include <cvt/cl/cl.hpp>
#include <cvt/util/Exception.h>

namespace cvt {
	namespace cl {

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
}
#endif
