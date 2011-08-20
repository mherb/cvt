#ifndef CVT_CLEXCEPTION_H
#define CVT_CLEXCEPTION_H
#include <cvt/cl/OpenCL.h>
#include <cvt/util/Exception.h>

namespace cvt {

		class CLException : public Exception
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
