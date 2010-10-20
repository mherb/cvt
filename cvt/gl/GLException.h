#ifndef GLEXCEPTION_H
#define GLEXCEPTION_H
#include <cvt/util/Exception.h>

namespace cvt {

		class GLException : cvt::Exception
		{
			public:
				GLException( ) throw( ) : _msg( "Unknown" ), _log( "" ) {};
				GLException( std::string s ) throw( ) : _msg( s ), _log( "" ) {};
				GLException( std::string s, std::string logmessage ) throw( ) : _msg( s ), _log( logmessage ) {};
				~GLException( ) throw( ) {};

				const char* what( ) const throw( ) { return _msg.c_str( ); };
				const char* log( ) const throw( ) { return _log.c_str( ); };

			private:
				std::string _msg;
				std::string _log;
		};
}
#endif
