#ifndef EXCEPTION_H
#define EXCEPTION_H
#include <exception>
#include <sstream>
#include <iostream>
#include <cstring>
#include <string>

namespace cvt {

#define CVTException( s ) ( ::cvt::Exception( s, __FILE__, __LINE__, __PRETTY_FUNCTION__ ) )

	class Exception : std::exception
	{
		public:
			Exception() throw(): msg( "Unknown" )
            {
#ifdef DEBUG
                    std::cerr << "EXCEPTION: " << msg << std::endl;
#endif
            }
        
			Exception( const char* s ) throw() : msg( s ) {}
			~Exception() throw() {}
			const char* what() const throw() { return msg.c_str(); }

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
                
#ifdef DEBUG
                std::cerr << "EXCEPTION: " << msg << std::endl;
#endif
			}


			Exception( const std::string & s, const char* file, size_t line, const char* func ) throw()
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
#ifdef DEBUG
                std::cerr << "EXCEPTION: " << msg << std::endl;
#endif
			}

		protected:
			std::string msg;
	};

}

#endif
