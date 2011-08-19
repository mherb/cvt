#ifndef CLEXCEPTION_H
#define CLEXCEPTION_H
#include <CL/opencl.h>
#include <Exception.h>

namespace cvt {

		class CLException : cvt::Exception
		{
			public:
				CLException( ) throw( ) : Exception( "Unknown" ), log( "" ) {};
				CLException( std::string s ) throw( ) : Exception( s ), log( "" ) {};
				CLException( cl_int error ) throw( );
				CLException( std::string location, cl_int error ) throw( );
				CLException( cl_int error, std::string logmessage ) throw( );
				~CLException( ) throw( ) {};

				const char* getLog( ) const throw( ) { return log.c_str( ); };

			private:
				std::string log;
		};
}
#endif
