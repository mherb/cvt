#ifndef CVT_CLEXCEPTION_H
#define CVT_CLEXCEPTION_H
#include <cvt/cl/OpenCL.h>
#include <cvt/util/Exception.h>

namespace cvt {
		/**
		  \ingroup CL
		  */
		class CLException : public Exception
		{
			public:
				CLException( ) throw( ) : Exception( "Unknown" ), log( "" ) {};
				CLException( const std::string& s ) throw( ) : Exception( s.c_str() ), log( "" ) {};
				CLException( cl_int error ) throw( );
				CLException( std::string location, cl_int error ) throw( );
				CLException( cl_int error, std::string logmessage ) throw( );
				~CLException( ) throw( ) {};

				const char* getLog( ) const throw( ) { return log.c_str( ); };

				static const char* CLErrorToString( cl_int error );
			private:
				std::string log;
		};
}
#endif
