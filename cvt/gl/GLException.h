#ifndef GLEXCEPTION_H
#define GLEXCEPTION_H
#include <cvt/util/Exception.h>

namespace cvt {

		class GLException : cvt::Exception
		{
			public:
				GLException( ) throw( ) : msg( "Unknown" ), log( "" ) {};
				GLException( std::string s ) throw( ) : msg( s ), log( "" ) {};
				GLException( std::string s, std::string logmessage ) throw( ) : msg( s ), log( logmessage ) {};
				~GLException( ) throw( ) {};

				const char* what( ) const throw( ) { return msg.c_str( ); };
				const char* getLog( ) const throw( ) { return log.c_str( ); };

			private:
				std::string msg;
				std::string log;
		};
}
#endif
