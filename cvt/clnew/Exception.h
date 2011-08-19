#ifndef EXCEPTION_H
#define EXCEPTION_H
#include <exception>
#include <sstream>
#include <cstring>

namespace cvt {

#define CVTException( s ) ( ::cvt::Exception( s, __FILE__, __LINE__, __PRETTY_FUNCTION__ ) )

	class Exception : std::exception
	{
		public:
			Exception() throw(): msg( "Unknown" ) {};
			Exception( std::string s ) throw() : msg( s ) {};
			~Exception() throw() {};
			const char* what() const throw() { return msg.c_str(); };

			Exception( int err, const char* file, size_t line, const char* func ) throw()
			{
				std::stringstream str;
				str << strerror( err );
				str << " ( FILE: ";
				str << file;
				str << " LINE: ";
				str << line;
				str << " FUNC: ";
				str << func;
				str << " )";
				msg = str.str();
			};


			Exception( std::string s, const char* file, size_t line, const char* func ) throw()
			{
				std::stringstream str;
				str << s;
				str << " ( FILE: ";
				str << file;
				str << " LINE: ";
				str << line;
				str << " FUNC: ";
				str << func;
				str << " )";
				msg = str.str();
			};

		protected:
			std::string msg;
	};

}

#endif
